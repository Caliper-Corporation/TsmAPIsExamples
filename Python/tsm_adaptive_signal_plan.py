import sys
import os
import traceback
import caliperpy


def main():
    dk = None
    log_file = None
    try:

        full_path = caliperpy.GetDirectory(sys.argv[0])
        app_name = "TransModeler"
        log_file = None if caliperpy.IsApplicationRunning(
            app_name) else full_path + "\\" + app_name + ".log"
        if isinstance(log_file, str):
            print(app_name + " will be managed by " + sys.argv[0])
            print("Log file: " + log_file)
            try:
                os.remove(log_file)
            except OSError:
                pass
        else:
            print(app_name + " is already running.")

        dk = caliperpy.TransModeler.connect(log_file=log_file)
        pinfo = dk.ShowArray(dk.GetProgram())
        print(pinfo)
        folder = dk.RunMacro("Tutorial Folder")
        print("Tutorial folder: " + folder)
        view_names = dk.GetViewNames()
        print(view_names)
        if app_name == "TransModeler":
            RunMgr = dk.CreateGisdkObject(None, "TSM.RunManager", None)
            Tsm = RunMgr.TsmApi
            Tsm.OpenFile(folder + "\\Hybrid Model\\Hybrid.smp")
            print("RunMgr = " + repr(RunMgr))
            print("Tsm = " + repr(Tsm))
            print("dk = " + repr(dk))
            RunMgr.SetDTASetting("Relative Gap", 0.001)
            RunMgr.SetDTASetting("First Gap", 1)
            RunMgr.SetDTASetting("Gap Interval", 10)
            RunMgr.SetDTAStartIteration(4)
            RunMgr.SetDTASetting("Max Iteration", 1)
            RunMgr.SetSimulationRunMode("Dynamic Traffic Assignment")
            status = Tsm.Run()  # This will not return until simulation is completed
            print("Simulation run status: " + str(status))
        else:
            print(dk.ShowArray(dk.GetProgram()))
        caliperpy.TransModeler.disconnect()

        if caliperpy.IsApplicationRunning(app_name):
            print(app_name + " continues to run.")
        else:
            print(app_name + " has stopped running")

    except Exception:
        traceback.print_exc()
        caliperpy.TransModeler.disconnect()
        if isinstance(log_file, str):
            with open(log_file, "r") as file_pointer:
                print(file_pointer.read())


if __name__ == "__main__":
    main()
