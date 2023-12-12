function Controller()
{
    var startMenuPage = gui.pageById(QInstaller.StartMenuSelection);
    if (startMenuPage) startMenuPage.title = qsTr("Setup Actions");
}
