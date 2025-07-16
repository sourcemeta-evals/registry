#include <gtest/gtest.h>
#include <curl/curl.h>
#include <curl/curlver.h>

TEST(CurlVersion, VerifyVersion814) {
  EXPECT_STREQ(LIBCURL_VERSION, "8.14.0-DEV");
  EXPECT_EQ(LIBCURL_VERSION_MAJOR, 8);
  EXPECT_EQ(LIBCURL_VERSION_MINOR, 14);
  EXPECT_EQ(LIBCURL_VERSION_PATCH, 0);
}

TEST(CurlVersion, VerifyVersionNum) {
  EXPECT_EQ(LIBCURL_VERSION_NUM, 0x080e00);
}

TEST(CurlVersion, VerifyInitialization) {
  CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);
  EXPECT_EQ(result, CURLE_OK);
  
  curl_version_info_data *version_info = curl_version_info(CURLVERSION_NOW);
  EXPECT_NE(version_info, nullptr);
  EXPECT_STREQ(version_info->version, "8.14.0-DEV");
  
  curl_global_cleanup();
}
