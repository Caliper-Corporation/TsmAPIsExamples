import sys
import os
import traceback
import caliperpy

def main():
    dk = None
    log_file = None
    app_name = "TransModeler"

    try:
        cur_folder = caliperpy.GetDirectory(sys.argv[0])
        log_file = None if caliperpy.IsApplicationRunning(app_name) else cur_folder + "\\" + app_name + ".log"

        if isinstance(log_file, str):
            print("Log file: " + log_file)

            try:
                os.remove(log_file)
            except OSError:
                pass

        tsm_connection = caliperpy.TransModeler
        tsm_dk = tsm_connection.connect(log_file=log_file)
        program = tsm_dk.GetProgram()
        tsm_info = tsm_dk.ShowArray(program)
        print(tsm_info)

        tsm_run_manager = tsm_dk.CreateGisdkObject(None, "TSM.RunManager", None)
        tsm_app = tsm_run_manager.TsmApi
        # Close in case there is an existing project.
        tsm_run_manager.CloseSimulationProject()
        tsm_app.OpenFile(cur_folder + "\\network\\Tower Rd and 56th Ave.smp")

        sim_step = tsm_app.StepSize
        start_time = tsm_app.StartTime
        end_time = tsm_app.EndTime
        start_time_str = tsm_app.TimeToString(start_time)
        end_time_str = tsm_app.TimeToString(end_time)
        print(f"Simulation StepSize = {sim_step}, StartTime = {start_time_str}, EndTime = {end_time_str}")

        print("tsm_run_manager = " + repr(tsm_run_manager))
        print("tsm_app = " + repr(tsm_app))
        print("tsm_dk = " + repr(tsm_dk))

        # Set desired time factor, tell TSM to run as fast as possible.
        tsm_app.DesiredTimeFactor = 1001

        initial_info = tsm_run_manager.GetInitialState()
        # Get warm up period in min
        warm_up = initial_info[1][1]

        # Intentionlly add up 1 sec to pass over the warm up end time
        warm_up_end_time = start_time + warm_up * 60 + 1;
        tsm_app.RunTo(warm_up_end_time )

        tsm_app.StepMode = 1
        sim_clock_time = warm_up_end_time
        cur_state  = 1 # red signal state
        while sim_clock_time < end_time :
            # Add per-simulation step processing here.
            """
            Here at each simulation step, we check the state of Sensor ID = 18,
            which maps to Detector Channel 27, east bound left turn lane
            """
            sensor = tsm_app.Network.Sensor(18)
            cur_time = tsm_app.CurrentTime
            cur_time_str = tsm_app.TimeToString(cur_time)
            if sensor.IsOccupied:
                print(f"At {cur_time_str}/{cur_time} Sensor 18 is ON!")
            else:
                print(f"At {cur_time_str}/{cur_time} Sensor 18 is OFF!")

            tsm_app.RunSingleStep()
            # End of per-simulation step processing.
            sim_clock_time = sim_clock_time + sim_step

        tsm_app.Reset()
        tsm_connection.disconnect()

        if caliperpy.IsApplicationRunning(app_name):
            print(app_name + " continues to run.")
        else:
            print(app_name + " has stopped running")

    except Exception:
        traceback.print_exc()
        tsm_connection.disconnect()

        if isinstance(log_file, str):
            with open(log_file, "r") as file_pointer:
                print(file_pointer.read())


if __name__ == "__main__":
    main()
