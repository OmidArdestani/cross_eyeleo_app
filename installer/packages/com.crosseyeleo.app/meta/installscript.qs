// installer/packages/com.crosseyeleo.app/meta/installscript.qs
// Component script: creates desktop shortcuts, launch entries,
// and OS startup (run-on-login) registration after the files have been installed.

function Component() {
    // Nothing to do in the constructor
}

Component.prototype.createOperations = function() {
    // Perform the default file-copy operations first
    component.createOperations();

    if (systemInfo.productType === "windows") {
        // ---- Windows: Start Menu + Desktop shortcuts ----
        component.addOperation(
            "CreateShortcut",
            "@TargetDir@\\CrossEyeLeoApp.exe",
            "@StartMenuDir@\\CrossEyeLeoApp.lnk",
            "workingDirectory=@TargetDir@",
            "description=Eye Exercise Reminder"
        );
        component.addOperation(
            "CreateShortcut",
            "@TargetDir@\\CrossEyeLeoApp.exe",
            "@DesktopDir@\\CrossEyeLeoApp.lnk",
            "workingDirectory=@TargetDir@",
            "description=Eye Exercise Reminder"
        );

        // ---- Windows: add to startup (HKCU Run key) ----
        component.addOperation(
            "Execute",
            "reg", "add",
            "HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
            "/v", "CrossEyeLeoApp",
            "/t", "REG_SZ",
            "/d", "@TargetDir@\\CrossEyeLeoApp.exe",
            "/f",
            "UNDOEXECUTE",
            "reg", "delete",
            "HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
            "/v", "CrossEyeLeoApp",
            "/f"
        );

        // ---- Windows: write installed version to registry (read by the app) ----
        component.addOperation(
            "Execute",
            "reg", "add",
            "HKCU\\Software\\CrossEyeLeoApp",
            "/v", "Version",
            "/t", "REG_SZ",
            "/d", "@Version@",
            "/f",
            "UNDOEXECUTE",
            "reg", "delete",
            "HKCU\\Software\\CrossEyeLeoApp",
            "/v", "Version",
            "/f"
        );

    } else if (systemInfo.kernelType === "linux") {
        // ---- Linux: .desktop entry in ~/.local/share/applications ----
        component.addOperation(
            "CreateDesktopEntry",
            "@HomeDir@/.local/share/applications/CrossEyeLeoApp.desktop",
            "Version=1.0\n" +
            "Type=Application\n" +
            "Name=CrossEyeLeoApp\n" +
            "GenericName=Eye Exercise Reminder\n" +
            "Exec=@TargetDir@/bin/run_crosseyeleo.sh\n" +
            "Comment=Cross-platform eye exercise reminder\n" +
            "Categories=Utility;Health;\n" +
            "Terminal=false\n" +
            "StartupNotify=true"
        );

        // ---- Linux: autostart entry in ~/.config/autostart ----
        component.addOperation(
            "CreateDesktopEntry",
            "@HomeDir@/.config/autostart/CrossEyeLeoApp.desktop",
            "Version=1.0\n" +
            "Type=Application\n" +
            "Name=CrossEyeLeoApp\n" +
            "GenericName=Eye Exercise Reminder\n" +
            "Exec=@TargetDir@/bin/run_crosseyeleo.sh\n" +
            "Comment=Cross-platform eye exercise reminder\n" +
            "Terminal=false\n" +
            "Hidden=false\n" +
            "X-GNOME-Autostart-enabled=true"
        );

        // ---- Linux: write installed version to config file (read by the app) ----
        // QSettings(NativeFormat, UserScope, "CrossEyeLeoApp") maps to
        // ~/.config/CrossEyeLeoApp.conf on Linux (distinct from the app's own
        // settings at ~/.config/CrossEyeLeo/CrossEyeLeoApp.conf).
        component.addOperation(
            "Execute",
            "/bin/sh", "-c",
            "mkdir -p \"@HomeDir@/.config\" && " +
            "printf '%s\\n' '[General]' 'Version=@Version@' > \"@HomeDir@/.config/CrossEyeLeoApp.conf\"",
            "UNDOEXECUTE",
            "/bin/sh", "-c",
            "rm -f \"@HomeDir@/.config/CrossEyeLeoApp.conf\""
        );

    } else if (systemInfo.productType === "osx") {
        // ---- macOS: LaunchAgent plist for login startup ----
        // PlistBuddy (available on all macOS since 10.5) creates the plist.
        // On reinstall the existing file is removed first so PlistBuddy
        // can build it from scratch without hitting "key already exists" errors.
        var plist    = "\"@HomeDir@/Library/LaunchAgents/com.crosseyeleo.app.plist\"";
        var execPath = "@TargetDir@/CrossEyeLeoApp.app/Contents/MacOS/CrossEyeLeoApp";
        var plistBuddyCmd =
            "mkdir -p \"@HomeDir@/Library/LaunchAgents\" && " +
            // On reinstall, remove the existing plist so PlistBuddy starts clean.
            "([ -f \"@HomeDir@/Library/LaunchAgents/com.crosseyeleo.app.plist\" ] && " +
            "rm -f \"@HomeDir@/Library/LaunchAgents/com.crosseyeleo.app.plist\"); " +
            // Build the plist from scratch
            "/usr/libexec/PlistBuddy" +
            " -c \"Add :Label string com.crosseyeleo.app\"" +
            " -c \"Add :ProgramArguments array\"" +
            " -c \"Add :ProgramArguments:0 string " + execPath + "\"" +
            " -c \"Add :RunAtLoad bool true\"" +
            " " + plist;
        component.addOperation(
            "Execute",
            "/bin/sh", "-c", plistBuddyCmd,
            "UNDOEXECUTE",
            "/bin/sh", "-c",
            "rm -f \"@HomeDir@/Library/LaunchAgents/com.crosseyeleo.app.plist\""
        );

        // ---- macOS: write installed version to user defaults (read by the app) ----
        // QSettings(NativeFormat, UserScope, "CrossEyeLeoApp") maps to
        // ~/Library/Preferences/CrossEyeLeoApp.plist on macOS.
        component.addOperation(
            "Execute",
            "/bin/sh", "-c",
            "defaults write CrossEyeLeoApp Version \"@Version@\"",
            "UNDOEXECUTE",
            "/bin/sh", "-c",
            "defaults delete CrossEyeLeoApp Version 2>/dev/null || true"
        );
    }
};
