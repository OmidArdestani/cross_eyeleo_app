// installer/config/installcontrol.qs
// Custom controller: brands the Introduction and Finished wizard pages
// with developer contact info and LinkedIn link.
//
// LinkedIn URL: update "omid-ardestani" below to match your actual profile slug.

function Controller() {
    // Automatically accept the "target directory already exists – overwrite?"
    // message box so that reinstalling into the same directory works without
    // requiring the user to manually choose a different path each time.
    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("installWarningOverwrite",   QMessageBox.Yes);
}

// ------------------------------------------------------------------
// Target directory page – silently accept "directory already exists"
// so that reinstalling over a previous installation works without
// asking the user to pick a different path.
// ------------------------------------------------------------------
Controller.prototype.TargetDirectoryPageCallback = function() {
    // Re-affirm the automatic answer in case the message box fires after
    // the constructor (some IFW versions raise it during page validation).
    installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory", QMessageBox.Yes);
    installer.setMessageBoxAutomaticAnswer("installWarningOverwrite",   QMessageBox.Yes);
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
