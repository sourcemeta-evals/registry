#!/bin/sh

set -o errexit
set -o nounset

TMP="$(mktemp -d)"
clean() { rm -rf "$TMP"; }
trap clean EXIT

cat << EOF > "$TMP/registry.json"
{
  "url": "https://sourcemeta.com/",
  "port": 8000,
  "schemas": {
    "example/schemas": {
      "base": "https://example.com/",
      "path": "./schemas"
    }
  }
}
EOF

mkdir "$TMP/schemas"

cat << 'EOF' > "$TMP/schemas/test.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/test.json",
  "title": "My title",
  "description": "My description"
}
EOF

cat << 'EOF' > "$TMP/schemas/no-title.json"
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "https://example.com/no-title.json"
}
EOF

export SOURCEMETA_REGISTRY_I_HAVE_A_COMMERCIAL_LICENSE=1
"$1" "$TMP/registry.json" "$TMP/output"

cat << 'EOF' > "$TMP/expected.json"
["/example/schemas/test.json","My title","My description"]
["/example/schemas/no-title.json","",""]
EOF

cat "$TMP/output/explorer/search.jsonl"

diff "$TMP/output/explorer/search.jsonl" "$TMP/expected.json"
