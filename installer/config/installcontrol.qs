// installer/config/installcontrol.qs
// Custom controller: brands the Introduction and Finished wizard pages
// with developer contact info and LinkedIn link.
//
// LinkedIn URL: update "omid-ardestani" below to match your actual profile slug.

function Controller() {
    // Automatically accept "target directory already exists" and "existing
    // installation detected" message boxes so that reinstalling over a previous
    // installation works without requiring the user to manually dismiss dialogs
    // or choose a different path.
    //
    // IFW raises different dialogs depending on the state of the target dir:
    //   OverwriteTargetDirectory  – directory exists but no prior IFW metadata
    //   installWarningOverwrite   – general overwrite warning (older IFW builds)
    //   maintenanceToolDetected   – directory contains a maintenance tool/metadata
    //                               from a previous IFW installation (the common
    //                               case on reinstall; this was the missing handler
    //                               that caused reinstalls to fail)
    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("installWarningOverwrite",   QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("maintenanceToolDetected",   QMessageBox.Yes);

    // Set the program to launch after installation (shown as a checkbox on the
    // Finished page).  Must be set early so IFW knows to show RunItCheckBox.
    if (systemInfo.productType === "windows") {
        installer.setValue("RunProgram", "@TargetDir@\\CrossEyeLeoApp.exe");
    } else if (systemInfo.kernelType === "linux") {
        installer.setValue("RunProgram", "@TargetDir@/bin/run_crosseyeleo.sh");
    } else if (systemInfo.productType === "osx") {
        installer.setValue("RunProgram",
            "@TargetDir@/CrossEyeLeoApp.app/Contents/MacOS/CrossEyeLeoApp");
    }
    installer.setValue("RunProgramDescription", "Launch CrossEyeLeoApp now");
}

// ------------------------------------------------------------------
// Target directory page – silently accept "directory already exists"
// and "maintenance tool detected" so that reinstalling over a previous
// installation works without asking the user to pick a different path.
// ------------------------------------------------------------------
Controller.prototype.TargetDirectoryPageCallback = function() {
    // Re-affirm all automatic answers in case the message boxes fire after
    // the constructor (some IFW versions raise them during page validation).
    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("installWarningOverwrite",   QMessageBox.Yes);
    // maintenanceToolDetected is raised when the target directory already
    // contains a maintenance tool / IFW metadata from a previous install.
    // Without this answer the reinstall fails even though OverwriteTargetDirectory
    // was already being handled.
    installer.setMessageBoxAutomaticAnswer("maintenanceToolDetected",   QMessageBox.Yes);
};

// ------------------------------------------------------------------
// Helper: stop any running instance of the application.
// Returns silently whether or not the app was running.
// ------------------------------------------------------------------
function stopRunningApp() {
    try {
        if (systemInfo.productType === "windows") {
            installer.execute("taskkill", ["/F", "/IM", "CrossEyeLeoApp.exe"]);
        } else {
            // pkill is available on both Linux and macOS.
            // Exit code 1 (no matching process) is normal and silently ignored.
            installer.execute("pkill", ["-x", "CrossEyeLeoApp"]);
        }
    } catch (e) {
        console.log("stopRunningApp failed (" + systemInfo.productType + "): " + e);
    }
}

// ------------------------------------------------------------------
// Helper: run the maintenance tool to cleanly uninstall the previous
// installation before the new files are copied.
// "purge" performs a fully silent, no-UI uninstall (IFW 4.3+).
//
// The path is passed as a direct OS argument array (not through a
// shell), so target directories containing spaces work correctly.
// Returns silently if no previous installation exists.
// ------------------------------------------------------------------
function purgePreviousInstallation(targetDir) {
    // Require an absolute path before constructing any executable paths,
    // guarding against an empty or unexpected value from the installer.
    if (!targetDir) return;
    if (systemInfo.productType === "windows") {
        if (!/^[A-Za-z]:\\/.test(targetDir)) return;
    } else {
        if (targetDir.charAt(0) !== "/") return;
    }

    try {
        if (systemInfo.productType === "windows") {
            installer.execute(
                targetDir + "\\CrossEyeLeoAppMaintenanceTool.exe", ["purge"]
            );
        } else if (systemInfo.productType === "osx") {
            installer.execute(
                targetDir + "/CrossEyeLeoAppMaintenanceTool.app" +
                            "/Contents/MacOS/CrossEyeLeoAppMaintenanceTool",
                ["purge"]
            );
        } else {
            installer.execute(
                targetDir + "/CrossEyeLeoAppMaintenanceTool", ["purge"]
            );
        }
    } catch (e) {
        console.log("purgePreviousInstallation failed (" + systemInfo.productType +
                    ", targetDir=" + targetDir + "): " + e);
    }
}

// ------------------------------------------------------------------
// Ready to Install page – before the user clicks "Install":
//   1. Stop any running instance of the app so its files can be
//      overwritten.
//   2. Run the existing maintenance tool (if present) to cleanly
//      remove the previous installation before copying new files.
//
// Both steps are fire-and-forget: errors are silently ignored so
// that a first-time install (no running app, no maintenance tool)
// works exactly the same as a reinstall.
// ------------------------------------------------------------------
Controller.prototype.ReadyToInstallPageCallback = function() {
    stopRunningApp();
    purgePreviousInstallation(installer.value("TargetDir"));
};

// ------------------------------------------------------------------
// Introduction page – show description + contact info
// ------------------------------------------------------------------
Controller.prototype.IntroductionPageCallback = function() {
    var widget = gui.currentPageWidget();
    if (!widget) return;

    var label = widget.MessageLabel;
    if (!label) return;

    label.setOpenExternalLinks(true);
    label.setText(
        "<b>CrossEyeLeo Eye Exercise Reminder</b><br><br>" +
        "A cross-platform application that sits in your system tray and " +
        "periodically prompts you to take short and long breaks with guided " +
        "eye exercises — helping you prevent eye strain during long screen " +
        "sessions.<br><br>" +
        "<b>Developer:</b>&nbsp;Omid Ardestani&nbsp;&nbsp;|&nbsp;&nbsp;" +
        "<a href=\"https://www.linkedin.com/in/omid-ardestani\">LinkedIn</a>" +
        "&nbsp;&nbsp;|&nbsp;&nbsp;" +
        "<a href=\"https://github.com/OmidArdestani/cross_eyeleo_app\">GitHub</a>"
    );
};

// ------------------------------------------------------------------
// Finished page – thank the user and show contact info again
// ------------------------------------------------------------------
Controller.prototype.FinishedPageCallback = function() {
    var widget = gui.currentPageWidget();
    if (!widget) return;

    // Ensure the "launch after installation" checkbox is visible and ticked.
    // The label text comes from RunProgramDescription set in the constructor.
    var runItCheckBox = widget.RunItCheckBox;
    if (runItCheckBox) {
        runItCheckBox.visible = true;
        runItCheckBox.checked = true;
    }

    var label = widget.MessageLabel;
    if (!label) return;

    label.setOpenExternalLinks(true);
    label.setText(
        "CrossEyeLeoApp has been installed successfully!<br><br>" +
        "Thank you for using CrossEyeLeoApp.<br><br>" +
        "<b>Developer:</b>&nbsp;Omid Ardestani<br>" +
        "<a href=\"https://www.linkedin.com/in/omid-ardestani\">" +
        "linkedin.com/in/omid-ardestani</a>&nbsp;&nbsp;|&nbsp;&nbsp;" +
        "<a href=\"https://github.com/OmidArdestani/cross_eyeleo_app\">" +
        "github.com/OmidArdestani/cross_eyeleo_app</a>"
    );
};
