import win32com.client


def main():
    # Create an instance of the automation server
    tsm = win32com.client.Dispatch("TransModeler.AutomationServer")

    # Call the RunMacro method
    tsm.RunMacro("OpenMap", "C:\\Users\\wuping\\Documents\\Caliper\\Maptitude 2023\\Tutorial\\BMP_SVR.MAP", None)


if __name__ == "__main__":
    main()
