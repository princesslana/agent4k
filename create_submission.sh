#!/bin/bash

# Create self-extracting compressed submission
cat > agent4k_submission << 'EOF'
#!/bin/bash
tail -n +6 "$0" | gzip -d > /tmp/engine.cpp
g++ -O2 /tmp/engine.cpp -o /tmp/engine
exec /tmp/engine "$@"
exit
EOF

# Append compressed source
gzip -9 -c engine_smart.cpp >> agent4k_submission

# Make executable
chmod +x agent4k_submission

echo "Created submission file: agent4k_submission"
wc -c agent4k_submission