#!/bin/bash
APP_DIR="$(cd "$(dirname "$0")/.." && pwd)"
export LD_LIBRARY_PATH="$APP_DIR/lib:$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="$APP_DIR/plugins:$QT_PLUGIN_PATH"
exec "$APP_DIR/bin/CrossEyeLeoApp" "$@"
