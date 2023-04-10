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
        log_file = None if caliperpy.IsApplicationRunning(app_name) else full_path + "\\" + app_name + ".log"
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
        print(dk.GetProgram())
        RunMgr = dk.CreateGisdkObject(None, "TSM.RunManager")
        sim_info = dk.GetSimulationClockStatus()
        print("simulation status", sim_info)

    except Exception:
        traceback.print_exc()
        caliperpy.TransModeler.disconnect()
        if isinstance(log_file, str):
            with open(log_file, "r") as file_pointer:
                print(file_pointer.read())


if __name__ == "__main__":
    main()
