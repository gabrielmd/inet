package org.omnetpp.inet.installer;

import java.io.File;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.installer.AbstractProjectInstaller;
import org.omnetpp.common.installer.ProjectDescription;


public class INETProjectInstaller extends AbstractProjectInstaller {
    public INETProjectInstaller(ProjectDescription projectDescription) {
        super(projectDescription);
    }

    @Override
    public void run(IProgressMonitor progressMonitor) {
        progressMonitor.beginTask("Installing project " + projectDescription.getName() + " into workspace", 5);
        File projectDistributionFile = downloadProjectDistribution(progressMonitor, projectDescription.getDistributionURL());
        progressMonitor.worked(1);
        if (progressMonitor.isCanceled()) return;
        File projectDirectory = extractProjectDistribution(progressMonitor, projectDistributionFile);
        progressMonitor.worked(1);
        if (progressMonitor.isCanceled()) return;
        IProject project = importProjectIntoWorkspace(progressMonitor, projectDirectory);
        progressMonitor.worked(1);
        if (progressMonitor.isCanceled()) return;
        openProject(progressMonitor, project);
        progressMonitor.worked(1);
        if (progressMonitor.isCanceled()) return;
        compileProject(progressMonitor, project);
        progressMonitor.worked(1);
        if (progressMonitor.isCanceled()) return;
        openEditor(progressMonitor, project.getFile("demo.html"));
        progressMonitor.done();
    }
}
