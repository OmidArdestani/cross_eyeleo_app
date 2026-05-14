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
