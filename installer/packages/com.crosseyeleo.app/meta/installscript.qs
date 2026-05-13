// installer/packages/com.crosseyeleo.app/meta/installscript.qs
// Component script: creates desktop shortcuts and launch entries
// after the files have been installed.

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
    }
    // macOS: the .app bundle is self-contained; no extra shortcut required.
};
