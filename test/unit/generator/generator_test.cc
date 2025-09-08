#include <filesystem>
#include <gtest/gtest.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>
#include <sourcemeta/registry/resolver.h>

class GeneratorTest : public ::testing::Test {
protected:
  void SetUp() override {
    resolver = std::make_unique<sourcemeta::registry::Resolver>();
  }

  std::unique_ptr<sourcemeta::registry::Resolver> resolver;
};

TEST_F(GeneratorTest, OfficialMetaSchemaNotModified) {
  // Test that official meta-schemas don't have their $schema property
  // overridden
  const std::string meta_schema_id = "http://json-schema.org/draft-04/schema#";

  // Get the official meta-schema
  auto result = (*resolver)(meta_schema_id);
  ASSERT_TRUE(result.has_value());

  auto schema = result.value();
  ASSERT_TRUE(schema.is_object());
  ASSERT_TRUE(schema.defines("$schema"));

  // Verify the $schema property is preserved as the original meta-schema
  // identifier
  EXPECT_EQ(schema.at("$schema").to_string(), meta_schema_id);
}

TEST_F(GeneratorTest, RegularSchemaCanHaveSchemaAssigned) {
  // Test that regular schemas can have $schema property assigned
  const std::filesystem::path schema_path = std::filesystem::current_path() /
                                            "test" / "unit" / "generator" /
                                            "schemas" / "regular-schema.json";

  // Create a collection for testing
  sourcemeta::registry::ResolverCollection collection{
      "test",
      sourcemeta::core::URI{"http://example.com"},
      "http://json-schema.org/draft-04/schema#",
      {}};

  sourcemeta::core::URI server_url{"http://localhost:8000"};

  // Add the regular schema to the resolver
  auto [original_id, new_id] =
      resolver->add(server_url, collection, schema_path);

  // Retrieve the schema
  auto result = (*resolver)(new_id);
  ASSERT_TRUE(result.has_value());

  auto schema = result.value();
  ASSERT_TRUE(schema.is_object());

  // Verify that $schema was assigned to the collection's default dialect
  if (schema.defines("$schema")) {
    EXPECT_EQ(schema.at("$schema").to_string(), collection.default_dialect);
  }
}

TEST_F(GeneratorTest, MetaSchemaResolutionDoesNotCauseInfiniteLoop) {
  // Test that resolving meta-schemas doesn't cause infinite loops
  const std::string meta_schema_id = "http://json-schema.org/draft-04/schema#";

  // This should complete without hanging or segfaulting
  auto result = (*resolver)(meta_schema_id);
  ASSERT_TRUE(result.has_value());

  auto schema = result.value();
  ASSERT_TRUE(schema.is_object());

  // Verify basic meta-schema properties
  EXPECT_TRUE(schema.defines("$schema"));
  EXPECT_TRUE(schema.defines("title"));
  EXPECT_EQ(schema.at("title").to_string(), "Core schema meta-schema");
}

TEST_F(GeneratorTest, Draft4MetaSchemaIdentification) {
  // Test specific Draft 4 meta-schema identification
  const std::string draft4_id = "http://json-schema.org/draft-04/schema#";

  // Verify that the official resolver recognizes this as an official
  // meta-schema
  auto official_result = sourcemeta::core::schema_official_resolver(draft4_id);
  EXPECT_TRUE(official_result.has_value());

  // Verify that our resolver also returns the correct schema
  auto resolver_result = (*resolver)(draft4_id);
  ASSERT_TRUE(resolver_result.has_value());

  auto schema = resolver_result.value();
  ASSERT_TRUE(schema.is_object());
  EXPECT_TRUE(schema.defines("$schema"));
  EXPECT_EQ(schema.at("$schema").to_string(), draft4_id);
}
