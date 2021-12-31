 
function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (installer.value("os") === "win") {
        component.addOperation("GlobalConfig",
                               "SourceForge",
                               "Molsketch",
                               "obabel-formats-dir",
                               installer.value("TargetDir"));
    }
}
