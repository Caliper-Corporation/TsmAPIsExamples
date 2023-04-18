from __future__ import annotations
from packaging import version
import win32com.client
import traceback
import os
import sys
import warnings
import psutil
import weakref
import numpy as np
import pandas as pd
import json 

# ---Package Files
import caliperpy.caliper3_dataframes as caliper3_dataframes

# ----the objects to be imported via *
# __all__ = dir(.) - ["Expression"]


def IsApplicationRunning(application_name) -> bool:
    """ Returns True if application_name is running in the current session. Usage: IsApplicationRunning("Maptitude")
    """
    process_names = {"Maptitude": "mapt.exe", "TransCAD": "tcw.exe", "TransModeler": "tsm.exe"}
    assert isinstance(application_name, str), "Input application_name must be a string, e.g. 'Maptitude'"
    process_name = None
    for app_name in process_names:
        if (application_name in app_name) or (app_name in application_name):
            process_name = process_names[app_name]
            break
    assert isinstance(process_name, str), "process_names does not contain an entry for " + application_name
    for process in psutil.process_iter():
        this_process_name = process.name()
        if this_process_name.lower() == process_name:
            return True
    return False


def GetDirectory(script_name) -> str:
    """ Returns the full path for a script file. Usage: GetDirectory(sys.argv[0])
    """
    assert isinstance(script_name, str), "Input script_name must be a string, e.g.: sys.argv[0]"
    path_name = os.path.dirname(script_name)
    return os.path.abspath(path_name)


def StopApplication(application_name) -> int:
    """ Force terminating application_name in the current session.
    """
    process_names = {"Maptitude": "mapt.exe", "TransCAD": "tcw.exe", "TransModeler": "tsm.exe"}
    assert isinstance(application_name, str), "Input application_name must be a string, e.g. 'Maptitude'"
    process_name = None
    for app_name in process_names:
        if (application_name in app_name) or (app_name in application_name):
            process_name = process_names[app_name]
            break
    if isinstance(process_name, str):
        process_name = process_name.replace(".exe", "", 1)
        try:
            killed = os.system('tskill "' + process_name + '" /a')
        except:
            killed = 0
        return killed
    return 0


class MatrixCurrency:
    """ a python class wrapping a transcad matrix currency
    with overloaded arithmetic operators
    """

    # dk_server: Gisdk COM server
    # mc_obj, dk_value: Gisdk matrix currency object

    def __init__(self, dk_server, mc_obj):
        self.dk_value = mc_obj
        self.dk_server = dk_server

    def assign_expression(self, exp) -> MatrixCurrency:
        """ evaluate expression: matrix_currency_3 := matrix_currency_1 operator matrix_currency_2  
        """
        new_dk_object = Expression(self, ":=", exp).Eval(self.dk_server)
        if new_dk_object is None:
            return None
        return MatrixCurrency(self.dk_server, new_dk_object)

    # overloaded matrix operators
    # https://www.programiz.com/python-programming/operator-overloading

    def __add__(self, other):
        """ +
        """
        return Expression(self, "+", other)

    def __sub__(self, other):
        """ -
        """
        return Expression(self, "-", other)

    def __mul__(self, other):
        """ *
        """
        return Expression(self, "*", other)

    def __truediv__(self, other):
        """ /
        """
        return Expression(self, "/", other)

    # in place matrix assignment operations

    def __iadd__(self, other):
        """ +=
        """
        new_exp = Expression(self, "+", other)
        e = Expression(self, ":=", new_exp).Eval(self.dk_server)
        return self

    def __isub__(self, other):
        """ -=
        """
        new_exp = Expression(self, "-", other)
        e = Expression(self, ":=", new_exp).Eval(self.dk_server)
        return self

    def __imul__(self, other):
        """ *=
        """
        new_exp = Expression(self, "*", other)
        e = Expression(self, ":=", new_exp).Eval(self.dk_server)
        return self

    def __idiv__(self, other):
        """ /=  
        """
        new_exp = Expression(self, "/", other)
        e = Expression(self, ":=", new_exp).Eval(self.dk_server)
        return self

    def __lshift__(self, other):
        """ assignment statement for matrix currencies << in python means := in the gisdk 
        """
        e = Expression(self, ":=", other).Eval(self.dk_server)
        return self

    def __ior__(self, other):
        """ assignment statement for matrix currencies |= in python means := in the gisdk 
        """
        e = Expression(self, ":=", other).Eval(self.dk_server)
        return self


class Vector(MatrixCurrency):
    """ a python class wrapping a transcad vector
    with overloaded arithmetic operators
    """

    # dk_server: Gisdk COM server
    # vector_obj, dk_value: Gisdk vector instance

    def __init__(self, dk_server, vector_obj):
        self.dk_server = dk_server
        self.dk_value = vector_obj

    def assign_expression(self, exp) -> Vector:
        """ evaluate expression: vector_3 = vector_1 operator vector_2  
        """
        new_dk_object = Expression(None, "=", exp).Eval(self.dk_server)
        if new_dk_object is None:
            return None
        return Vector(self.dk_server, new_dk_object)


class Expression:
    """ a gisdk matrix currency or vector expression that parses as "left operation right"
    """

    def __init__(self, left, operation, right):
        self.left = left
        self.operation = operation
        self.right = right

    # overloaded expression operators

    def __add__(self, other):
        """ +
        """
        return Expression(self, "+", other)

    def __sub__(self, other):
        """ -
        """
        return Expression(self, "-", other)

    def __mul__(self, other):
        """ *
        """
        return Expression(self, "*", other)

    def __truediv__(self, other):
        """ /
        """
        return Expression(self, "/", other)

    def Parse(self, exp='', args=[], add_paren=False):
        """ prepare a gisdk assignment statement
        returns: exp, args
        exp will be a literal expression of type: "args[1] = args[2] operation args[3]"
        args will be an array of dk values
        args[1] can be initially none if assigning a new value 
        """
        # parse the left side
        if (isinstance(self.left, Expression)):
            (left_exp, args) = self.left.Parse('', args, False)
        else:
            left_exp = " args[" + str(len(args) + 1) + "] "
            args.append(_list(self.left))
        # add the operation
        if (add_paren):
            exp += " ( "
        exp += left_exp + " " + self.operation + " "
        # parse the right side
        if (isinstance(self.right, Expression)):
            (right_exp, args) = self.right.Parse('', args, False)
        else:
            right_exp = " args[" + str(len(args) + 1) + "] "
            args.append(_list(self.right))
        exp += right_exp
        if (add_paren):
            exp += " ) "
        # returns updated expression string and args list
        return (exp, args)

    def Eval(self, dk_server):
        """ Evaluate the gisdk expression "self.left self.operation self.right"
        """
        if (self.operation is None):
            return None
        if (self.right is None) and (self.left is None):
            return None
        # If operation is an assignment and right is None then result is None
        if (self.operation == '=') and (self.right is None):
            return None
        # The left side of a matrix assignment must exist before we fill it
        if (self.operation == ':=') and (self.left is None):
            return None
        # Else if either right or left is None then result is None
        if (self.operation != '=') and ((self.left is None) or (self.right is None)):
            return None
        args = []
        exp = ""
        exp, args = self.Parse(exp, args, False)
        if (len(args) > 0 and len(exp) >= 5):
            return dk_server.Eval(exp, args)
        return None


def _list_keyval(key, val) -> list:
    """ returns a list, or array of: [ key, val ] (we can only pass lists as input arguments to the Gisdk)
    """
    return [key, _list(val)]


def _list(obj):
    """ returns the dk_value or a list of dk_values corresponding to a python object. (we can only pass lists as input arguments to the Gisdk).
    """
    weakref_obj = None
    try:
        weakref_obj = weakref.ref(obj)
    except TypeError:
        weakref_obj = obj
    if hasattr(obj, "dk_value"):
        return _list(obj.dk_value)
    elif type(obj) in [list, tuple, slice]:
        return [_list(item) for item in obj]
    elif type(obj) in [dict]:
        return [_list_keyval(index, item) for (index, item) in obj.items()]
    return obj


class _GisdkMethod:
    """ Implement a callable function for each method of an instance of a Gisdk class or an instance of a .NET class
    """

    def __init__(self, gisdk_obj, method_name):
        """ called when user creates an instance of this class
        """
        self.__dict__["gisdk_object"] = gisdk_obj
        self.__dict__["method_name"] = method_name

    def __call__(self, *args):
        """ called when user calls gisdk_object.method_name(args)
        """
        return self.__dict__["gisdk_object"].apply(self.__dict__["method_name"], *args)


class GisdkObject:
    """ A Python object wrapping a GISDK class instance or a .NET class instance
    Usage: dk = caliper.Gisdk("TransCAD") 
    c = dk.CreateGisdkObject("gis_ui","calculator","test message",[ 1 , 2 , 3]) 
    date = dk.CreateManagedObject("System","DateTime",[1980,1,1,0,0,0,0])
    m = c.message
    a = c.args
    a.message = m
    a.args = [ 2 , 3 , 4]
    c1 = c.add(10,20)
    d1 = c.subtract(20,10)
    c_info = dk.GetClassInfo(c)
    d_info = dk.GetClassInfo(date)
    """

    def __init__(self, gisdk_server=None, gisdk_object=None, class_name=None, ui_name=None):
        """ Wrap an internal Gisdk object instance or a .NET object instance into a Python object of type GisdkObject() with callable methods and field names 
        """
        assert isinstance(gisdk_server, Gisdk), "The first argument to GisdkObject() must be an instance of the Gisdk"
        assert not gisdk_object is None, "The second argument to GisdkObject() must be an Gisdk object instance or a .NET object instance"
        method_names = None
        assembly_name = None
        interface_name = None
        dk_object = gisdk_object
        if isinstance(gisdk_object, GisdkObject):
            dk_object = gisdk_object.dk_value
            assert not dk_object is None, "The input object passed to GisdkObject() is empty"
        if class_name is None:
            class_info = gisdk_server.GetClassInfo(dk_object)
            if isinstance(class_info, dict):
                if "ClassName" in class_info:
                    class_name = class_info["ClassName"]
                if "FullClassName" in class_info:
                    class_name = class_info["FullClassName"]
                if "MethodNames" in class_info:
                    method_names = class_info["MethodNames"]
                if "AssemblyName" in class_info:
                    assembly_name = class_info["AssemblyName"]
                if "InterfaceName" in class_info:
                    interface_name = class_info["InterfaceName"]
            assert isinstance(class_name, str), class_info["Message"] if "Message" in class_info else "Cannot derive a type for the input object passed to GisdkObject()"
        else:
            # the input object must be an instance of a GISDK class
            method_names = gisdk_server.GetClassMethodNames(class_name)
        assert isinstance(class_name, str), "Cannot derive a type for the input object passed to GisdkObject()"
        self.__dict__["class_name"] = class_name
        self.__dict__["ui_name"] = ui_name
        self.__dict__["assembly_name"] = assembly_name
        self.__dict__["interface_name"] = interface_name
        self.__dict__["dk_value"] = dk_object
        self.__dict__["dk_server"] = gisdk_server
        self.__dict__["method_names"] = method_names
        for method_name in self.__dict__["method_names"]:
            self.__dict__[method_name] = _GisdkMethod(self, method_name)

    def __str__(self):
        my_type = "GisdkObject"
        my_module = self.__dict__["ui_name"] or "gis_ui"
        if isinstance(self.__dict__["assembly_name"], str):
            my_type = "DotNetObject"
            my_module = self.__dict__["assembly_name"]
        if isinstance(self.__dict__["interface_name"], str):
            my_type = "ComObject"
            my_module = self.__dict__["interface_name"]
        return "<" + my_type + " class: " + self.__dict__["class_name"] + " in: " + my_module + ">"

    def __repr__(self):
        return self.__str__()

    def apply(self, method_name, *args):
        """ Apply a method for a gisdk object or a managed object, e.g. sum = calculator.apply("add",2,3), or sum = calculator.add(2,3)
        """
        assert not self.__dict__["dk_server"] is None, "Cannot call method: missing connection to the Gisdk."
        assert not self.__dict__["dk_value"] is None, "Cannot call method: Gisdk object is None."
        assert isinstance(method_name, str), "Cannot call method: input method_name must be a string."
        o = self.__dict__["dk_value"]
        # result = self.__dict__["dk_server"].apply("apply_method", "gis_ui", o, method_name, *(_list(arg) for arg in args))
        result = self.__dict__["dk_server"].apply("apply_method", "gis_ui", o, method_name, args)
        if (self.__dict__["dk_server"].IsGisdkObject(result)):
            return GisdkObject(self.__dict__["dk_server"], result)
        return result

    def __getattr__(self, attribute_name):
        """ get any attribute stored in a gisdk object. It only gets called when a nonexistent attribute is accessed
        """
        if (not isinstance(attribute_name, str)):
            return None
        if (attribute_name == "__members__"):
            return ["ui_name", "class_name", "dk_value", "dk_server"]
        if (attribute_name == "__methods__"):
            return ["apply"]
        if ("__" in attribute_name):
            return None
        if ((not self.__dict__["dk_value"] is None) and (not self.__dict__["dk_server"] is None) and (
                not attribute_name in ["_oleobj_", "class_name", "ui_name", "dk_value", "apply", "dk_server"])):
            o = self.__dict__["dk_server"].RunMacro("get_attribute", self.__dict__["dk_value"], attribute_name)
            if (self.__dict__["dk_server"].IsGisdkObject(o)):
                return GisdkObject(self.__dict__["dk_server"], o)
            return o
        result = self.__dict__[attribute_name]
        return result

    def __setattr__(self, attribute_name, attribute_value):
        """ set any attribute stored in a gisdk object.  It allows you to define behavior for assignment to an attribute regardless of whether or not that attribute exists
        """
        if attribute_name in self.__dict__["method_names"]:
            raise ValueError(attribute_name + " is a method name in this GISDK object and cannot be set.")
        if not isinstance(attribute_name, str):
            return None
        if (attribute_name not in ["class_name", "ui_name", "dk_value", "dk_server"]) and (not self.__dict__["dk_value"] is None) and (not self.__dict__["dk_server"] is None):
            return self.__dict__["dk_server"].RunMacro("set_attribute", self.__dict__["dk_value"], attribute_name, attribute_value)
        elif not attribute_name == "apply":
            self.__dict__[attribute_name] = attribute_value


class Gisdk:
    """ A Python class for managing Gisdk functions, Gisdk macros, Gisdk class instances and .NET managed objects.
    Usage:
    dk = caliperpy.Gisdk(application_name = 'TransCAD')
    """
    __obj = None
    __function_stack = []
    __application_name = "TransCAD"
    __process_names = {"Maptitude": "mapt.exe", "TransCAD": "tcw.exe", "TransModeler": "tsm.exe"}
    __ui_name = "gis_ui"

    def __init__(self, application_name="TransCAD", log_file=None, search_path=None):
        """ Create a Gisdk application object.
        application_name can be one of: 'TransCAD' , 'Maptitude' , 'TransModeler' , 'TransModeler.AutomationServer.2' , ...
        Setting a log_file different than None will flush all messages and message boxes to a log file
        """
        assert isinstance(application_name, str), "Input application_name be a string."
        process_name = self.GetProcessName(application_name)
        assert isinstance(process_name, str), "Input application_name must contain 'Maptitude', 'TransCAD' or 'TransModeler'"
        self.__obj = None                     # store a handle to the Gisdk COM object
        self.__function_stack = []            # store the names of the next Gisdk function to execute
        self.__application_name = application_name  # default com object
        self.__visible = False  # set self.__visible to True if attaching my script to a process that is already running in the current session.
        self.__ui_name = "gis_ui"
        for process in psutil.process_iter():
            this_process_name = process.name()
            if this_process_name.lower() == process_name:
                self.__visible = True
                break
        self.__log_file = log_file
        self.__search_path = search_path
        self.__client_data = None
        self.Open(application_name, log_file, search_path, self.__visible)

    def GetProcessName(self, application_name):
        """ Returns the process name corresponding to application_name (e.g. 'mapt.exe').
        Input application_name can be: "Maptitude", "TransModeler", "TransModeler.AutomationServer.1", ...
        """
        for app_name in self.__process_names:
            if (app_name in application_name) or (application_name in app_name):
                return self.__process_names[app_name]
        return None

    def Open(self, application_name="TransCAD", log_file=None, search_path=None, visible=False):
        """ Open or re-open a connection to the Caliper COM object. If visible is True, attach to a visible process, else create a new 'silent' process.
        """
        assert isinstance(application_name, str), "Input application_name must be a string containing Maptitude, TransCAD or TransModeler"
        if self.__obj is not None:
            self.Close()
        self.__obj = None                   # store a handle to the Gisdk COM object
        self.__function_stack = []          # store the names of the next Gisdk function to execute
        if isinstance(log_file, str) and ("\\" not in log_file):
            log_file = os.environ["TEMP"] + "\\" + log_file
        self.__application_name = application_name
        com_server_name = self.__application_name
        if "." not in com_server_name:
            com_server_name += ".AutomationServer"
        try:
            self.__obj = win32com.client.Dispatch(com_server_name)
            self.com_client_obj = self.CreateGisdkObject("gis_ui", "COM.Client")
            client_data = self.__obj.RunUIMacro("init_client", "gis_ui", log_file, search_path)
            if client_data is not None:
                self.__client_data = dict(client_data)
                self.__client_data["TempFolder"] = self.__obj.RunMacro("GetTempPath")
        except:
            print("Cannot access COM object: " + com_server_name)
            traceback.print_exc()
            raise

    def GetClientData(self) -> dict:
        if self.__client_data is not None:
            return self.__client_data
        return None

    def GetMessageFile(self) -> dict:
        temp_folder = self.GetTempFolder()
        if temp_folder is None:
            return None
        return temp_folder + "script-message.json"

    def GetTempFolder(self) -> str: 
        if self.__obj is not None and self.__client_data is not None:
            return self.__client_data["TempFolder"]

    def PostMessage(self,topic_str: str, body_dict: dict) -> bool:
        """ Post the message: { topic: topic_str, body: body_dict } to the program temp folder 
        """
        temp_folder = self.GetTempFolder()
        if temp_folder is None:
            return False 
        data = dict({ 'topic': topic_str, 'body': dict(body_dict) })        
        with open(self.GetMessageFile(), 'w') as json_file:
            json.dump(data,json_file)
        return True 

    def IsConnected(self):
        return self.__obj is not None

    def __str__(self):
        return "<Gisdk " + self.__application_name + ">"

    def __repr__(self):
        return "<Gisdk " + self.__application_name + ">"

    def __del__(self):
        """ cleanup my resources before being garbage collected
        """
        return self.Close()

    def TypeOf(self, obj):
        """ Return one of the Gisdk object types: None , "string" , "int" , "double" , "object" , "oleobject" , "managedobject" , "coord" , "scope" , "circle" , ...
        """
        if obj is None:
            return None
        obj_type = self.__obj.RunUIMacro("get_object_type", "gis_ui", obj)
        if obj_type is "null":
            return None
        return obj_type

    def IsGisdkObject(self, obj):
        """ Returns True if obj is an instance of a GISDK Class, or of a .NET Managed Class.
        """
        if obj is None:
            return False
        if isinstance(obj, GisdkObject):
            return True
        obj_type = self.TypeOf(obj)
        if obj_type in ["object", "managedobject"]:
            return True
        return False

    def GetGisdkObject(self, obj):
        """ Returns a dynamic GISDK object with callable methods if obj can be cast to an instance of a GISDK class or a .NET managed object 
        """
        return GisdkObject(self, obj)

    def GetClassInfo(self, obj):
        """ Returns a dict with keys: Error, Message, ClassName, FullClassName, AssemblyName, MethodNames, FieldNames if the input object is an instance of a GISDK class or a .NET managed class
        """
        if obj is None:
            return None
        info = None
        if isinstance(obj, GisdkObject) and (not obj.dk_value is None):
            info = self.__obj.RunUIMacro("get_class_info", "gis_ui", obj.dk_value)
            if not info is None:
                return dict(info)
            return None
        if self.IsGisdkObject(obj):
            info = self.__obj.RunUIMacro("get_class_info", "gis_ui", obj)
        if not info is None:
            return dict(info)
        return None

    def Close(self):
        """ attempt to terminate the com mapping server object if I started it myself
        This action cannot be undone.
        """
        if (not self.__obj is None) and (not self.com_client_obj is None):
            self.com_client_obj.Cleanup()
            self.com_client_obj = None
        # if (not self.__obj is None) and (self.__visible):
        #     self.SetErrorLogFileName(None)
        if (self.__obj is None) or (self.__visible):
            return
        try:
            self.__obj.RunUIMacro("terminate_program", "gis_ui")
        except:
            self.__obj = None
        finally:
            self.__obj = None

    def Eval(self, exp, args):
        """ Evaluate the gisdk matrix or vector expression exp: "args[1] << args[2] + args[3]"
        """
        if (len(args) > 0 and len(exp) >= 5):
            return self.RunMacro("evaluate_expression", exp, args)
        return None

    # every gisdk attribute must be a method call
    # support gisdk.FunctionName(*args) syntax by executing self.__gisdk_function(*args)

    def __getattr__(self, function_name):
        """ Call a GISDK function. Returns a gisdk function object
        """
        assert isinstance(function_name, str), str(function_name) + ' Input function_name must be a string'
        assert function_name.find("__") == -1, function_name + " A Gisdk instance or a private attribute is not callable as a function. "
        if (len(self.__function_stack) < 10):
            self.__function_stack.append(function_name)
        return self.__gisdk_function

    def __gisdk_function(self, *args):
        """ Execute a gisdk function
        """
        if (len(self.__function_stack) < 1):
            return None
        function_name = self.__function_stack.pop()
        if (not isinstance(function_name, str)):
            return None
        assert function_name.find("__") == -1, "A Gisdk instance or a private attribute is not callable as a function."
        try:
            o = self.__obj.RunMacro(function_name, *(_list(arg) for arg in args))
        except:
            self.print_exception("Gisdk error executing function " + function_name + "(" + str(args) + ")")
            raise
        return o

    def print_exception(self, message):
        if not self.__log_file is None:
            fh = open(self.__log_file, "a")
            fh.write(message)
            fh.write("\n")
            fh.write(traceback.format_exc())
            fh.close()
            fh = None
        print(message)
        traceback.print_exc()

    def CreateObject(self, class_name, ui_name, *args):
        """ Deprecated: Use CreateGisdkObject() instead. Create an instance of a GISDK class defined in the compiled Gisdk module, or in the current UI module
        """
        assert isinstance(class_name, str), 'Input class_name must be a string'
        gisdk_object = self.RunMacro("create_object", ui_name, class_name, *args)
        assert (not gisdk_object is None), "Cannot create an instance of the Gisdk class " + class_name + ((" in module " + ui_name) if isinstance(ui_name, str) else "")
        return GisdkObject(self, gisdk_object, None, ui_name)

    def CreateGisdkObject(self, ui_name, class_name, *args):
        """ Create an instance of a GISDK class defined in the compiled Gisdk module ui_name. If ui_name is None then it defaults to the current Gisdk module
        """
        assert isinstance(class_name, str), 'Input class_name must be a string'
        assert (ui_name is None) or isinstance(ui_name, str), 'Input ui_name must be a string'
        gisdk_object = self.RunMacro("create_object", ui_name, class_name, *args)
        assert (not gisdk_object is None), "Cannot create an instance of the Gisdk class " + class_name + ((" in module " + ui_name) if isinstance(ui_name, str) else "")
        return GisdkObject(self, gisdk_object, None, ui_name)

    def CreateManagedObject(self, assembly_name, class_name, *args):
        """ Create an instance of a .NET managed class defined in assembly_name. If assembly_name is None then it defaults to "System".
        """
        assert isinstance(class_name, str), 'Input class_name must be a string'
        if (assembly_name is None):
            assembly_name = "System"
        assert isinstance(assembly_name, str), 'Input assembly_name must be a string'
        managed_object = self.RunMacro("create_managed_object", assembly_name, class_name, *args)
        assert (not managed_object is None), "Cannot create an instance of the .NET class " + class_name + " in assembly " + assembly_name
        return GisdkObject(self, managed_object)

    def CreateComObject(self, com_class_name, *args):
        """ Create an instance of a COM class.
        """
        assert isinstance(com_class_name, str), 'Input com_class_name must be a string'
        com_object = win32com.client.Dispatch(com_class_name)
        assert (not com_object is None), "Cannot create an instance of the COM class " + com_class_name
        return com_object

    def SetAlternateInterface(self, ui_name=None):
        """ Set the alternate interface for running macros via RunMacro 
        """
        if ui_name is None or (isinstance(ui_name, str) and ui_name is "gis_ui"):
            self.__ui_name = "gis_ui"
            return "gis_ui"
        if isinstance(ui_name, str) and not os.path.exists(ui_name):
            raise FileNotFoundError("Cannot find Gisdk compiled UI " + ui_name)
        self.__ui_name = ui_name
        return ui_name

    def GetInterface(self):
        return self.__ui_name

    def Macro(self, macro_name, *args):
        """ Execute a gisdk macro defined in the compiled Gisdk module ui_name, or the current ui module.
        """
        assert isinstance(macro_name, str), "Input macro_name must be a string"
        return self.apply(macro_name, self.__ui_name, *args)

    def RunMacro(self, macro_name, *args):
        """ Execute a gisdk macro defined in the compiled Gisdk module ui_name, or the current ui module.
        """
        assert isinstance(macro_name, str), "Input macro_name must be a string"
        return self.apply(macro_name, self.__ui_name, *args)

    def apply(self, macro_name, ui_name="gis_ui", *args):
        """ Execute a gisdk macro in a specific UI. This method name is more Python-style.
        """
        assert isinstance(macro_name, str), "Input macro_name must be a string"
        assert (ui_name is None) or isinstance(ui_name, str), "Input ui_name must be None or a string"
        try:
            o = self.__obj.RunUIMacro(macro_name, ui_name, *(_list(arg) for arg in args))
        except:
            self.print_exception("Gisdk error executing RunMacro('" + macro_name + "','" + ui_name + "'," + str(args) + ")")
            raise
        return o

    # wrappers around gisdk matrix and vector functions

    def CreateMatrixCurrency(self, m, core, rowindex, colindex, options):
        """ returns a python transcad matrix currency object
        """
        mc = self.__obj.RunMacro("CreateMatrixCurrency", m, core, rowindex, colindex, _list(options))
        return MatrixCurrency(self, mc)

    def GetMatrixEditorCurrency(self, editor_name):
        """ returns a python transcad matrix currency object
        """
        mc = self.__obj.RunMacro("GetMatrixEditorCurrency", editor_name)
        return MatrixCurrency(self, mc)

    def GetDataVector(self, view_set, field, options=None):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("GetDataVector", view_set, field, _list(options))
        return Vector(self, v)

    def GetMatrixVector(self, mc, options):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("GetMatrixVector", _list(mc), _list(options))
        return Vector(self, v)

    def Vector(self, length, type, options=None):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("Vector", length, type, _list(options))
        return Vector(self, v)

    def ArrayToVector(self, arr, options=None):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("ArrayToVector", _list(arr), _list(options))
        return Vector(self, v)

    def a2v(self, arr, options=None):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("ArrayToVector", _list(arr), _list(options))
        return Vector(self, v)

    def SortVector(self, v, options=None):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("SortVector", _list(v), _list(options))
        return Vector(self, v)

    def CopyVector(self, v):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("CopyVector", _list(v))
        return Vector(self, v)

    def CumulativeVector(self, v, options=None):
        """ returns a python transcad vector object
        """
        v = self.__obj.RunMacro("CumulativeVector", _list(v), _list(options))
        return Vector(self, v)

    def ShowArray(self, obj, open_symbol="[", delimiter=",", close_symbol="]"):
        """ format a gisdk array to a string 
        """
        arr = [obj] if (type(obj) not in [list, tuple, dict, slice]) else obj
        x = self.RunMacro("format_array", _list(arr), open_symbol, delimiter, close_symbol)
        return x

    def Dump(self, obj):
        """ dump the gisdk value stored in obj to the python.log file
        """
        x = self.ShowArray(obj)
        self.RunMacro("log_message", x)
        return x

    def L(self, obj):
        return self.Dump(obj)

    # ----Functions to access the bin tables via python

    def WriteBinFromDataFrame(self, df: pd.core.frame.DataFrame, outFilename: str, coding='cp1252') -> str:
        '''
            WriteBinFromDataFrame(df, outFilename):
                df: the dataframe to be written out
                outFilname: the file name of th bin table including ".bin" extension

            Writes out the bin file from a dataframe and the corresponding dcb file
            Returns the output binary file 
        '''

        # ----Checking the pd version
        if version.parse(pd.__version__) < version.parse("1.0.1"):
            warnings.warn('Please use pandas version later than 1.0.1.', ImportWarning)

        df2 = df.copy()  # --making changes to the copy so that original df does not change

        # --list of columns types
        col_types = [str(i).lower() for i in df2.dtypes.values]
        # ---Checking for unsupported dtypes
        supported_types = ('int', 'float', 'object', 'datetime64[ns]')
        for col_type in col_types:
            if not any(x in col_type for x in supported_types):
                raise TypeError("The input dataframe contains an unsupported type: " + col_type + ". Supported types are " + str(supported_types))

        # --list of dt columns names
        dt_cols = list(df.dtypes[df.dtypes == "datetime64[ns]"].index)

        # --converting datetime to date and time integercols
        df2 = caliper3_dataframes.set_dt_values(df2)

        col_list = list(df2.dtypes.index)
        col_types = [str(i).lower() for i in df2.dtypes.values]
        # ---Setting datatype for string and int columns
        for i in range(len(col_list)):
            col_type = col_types[i]
            col_name = col_list[i]
            #print(col_name, col_type)
            if col_type == 'object':
                try:
                    max_elemLen = int(df[~df[col_name].isna()][col_name].str.len().max())
                except:
                    max_elemLen = 10
                col_types[i] = 'S' + str(max_elemLen + 1)  # buffer length TODO: needed?

            elif col_type == 'int64' or col_type == 'Int64':
                df2[col_name] = df2[col_name].astype('int32')
                col_types[i] = 'int32'

        # ---Setting the NA values for TC to recognize
        df2 = caliper3_dataframes.set_na_str_values(df2, coding)

        # --col_dypes for numpy mapping and writing bin file
        col_dtypes = {col_list[i]: col_types[i] for i in range(len(col_list))}
        # print("+++ col_dtypes to read as numpy": col_dtypes)
        np_records = df2.to_records(index=False, column_dtypes=col_dtypes)
        np_records.tofile(outFilename)

        # ---Writing the DCB file
        nBytesPerRow = len(np_records[0].tobytes())
        caliper3_dataframes.write_dcb_file(outFilename, df2, col_dtypes, dt_cols, nBytesPerRow, coding)

        return outFilename

    # ======================================================================================================

    def GetDataFrameFromBin(self, filename: str, debug_msgs: bool = False) -> pd.core.frame.DataFrame:
        '''
            GetDataFrameFromBin(filename, debu_msgs = False)

                    filename : name of the file with ".bin" extension
                    debug_msgs (boolean): returns the interpreted column types among other messages
                    returns a pandas Dataframe from Caliper Gisdk bin table
        '''

        # ----Checking the pd version
        if version.parse(pd.__version__) < version.parse("1.0.1"):
            warnings.warn('Please use pandas version later than 1.0.1.', ImportWarning)

        # converting the FFB file to latest format
        if caliper3_dataframes.is_old_bin_format(filename):
            caliper3_dataframes.convert_bin_to_newformat(filename)
            
        # datatypes and row length in bytes
        dt_list, tcType_list, nBytesPerRow, coding = caliper3_dataframes.read_dtypes(filename, debug_msgs)

        #CODING = 'windows-1252'
        CODING = coding
        del_pattern = b'\x91\x8b\x4a\x5c\xbc\xdb\x4f\x14\x63\x23\x7f\x78\xa6\x95\x0d\x27'
        del_pattern = del_pattern[:min(nBytesPerRow, 16)]

        # TODO: assert might not be needed as there cannot be deleted records in tables with < 5 bytes per row
        assert nBytesPerRow >= 5, '''Cannot handle table with rows having  less than 5 bytes yet !!
                                    Current row byte length {0}'''. format(nBytesPerRow)

        # ---Cleaning the file to remove the deleted records

        # reading file content as byte array
        with open(filename, mode='rb') as inFile:
            fileContent = inFile.read()
        file_array = bytearray(fileContent)

        # removing the deleted records
        del_count = 0
        while file_array.find(del_pattern) > -1:
            pos = file_array.find(del_pattern)
            file_array = file_array[:pos] + file_array[pos + nBytesPerRow:]
            del_count += 1

        if debug_msgs:
            print("+++ Infered byte length of row: {0}".format(nBytesPerRow))
            print("+++ Found {0} deleted rows".format(del_count))

        # writing the "corrected/cleaned" byte data
        if del_count > 0:
            with open(filename, "wb") as outFile:
                outFile.write(file_array)

        # ---Reading the cleaned file as np-array and then df
        dt = np.dtype(dt_list)
        data = np.fromfile(filename, dtype=dt)
        df = pd.DataFrame.from_records(data)
        if debug_msgs:
            print("+++ Number of columns: ", len(dt_list))
            print("+++ Column data types infered: \n", dt_list)

        # ---Decoding the character fields
        char_columns = [item[0] for item in dt_list if 'S' in item[1]]
        # print(f"CODING: {CODING}")
        for col in char_columns:
            df[col] = df[col].str.decode(CODING).str.strip()
            # --putting in None where values are ''
            df[col].mask(df[col] == '', None, inplace=True)

        df = caliper3_dataframes.read_na_values(df, dt_list)

        # ---handing date and time fields
        df = caliper3_dataframes.read_datetime(df, dt_list, tcType_list)

        return df

    # convert tuples into dictionary recursively
    def TupleToDict(self, tup):
        new_dict = {}
        for element in tup:
            if isinstance(element[1], tuple) and isinstance(element[1][0], tuple):
                new_dict[element[0]] = self.TupleToDict(element[1])          
            else:
                new_dict[element[0]] = element[1]

        return new_dict

    # ======================================================================================================


class TransCAD:

    """ Static instance of the TransCAD Gisdk shared by all of my functions
    """
    __dkInstance = None

    @staticmethod
    def connect(log_file: str = None) -> Gisdk:
        """ Return the TransCAD Gisdk object. Can be called multiple times.
        """
        if log_file is None:
            log_file = os.getcwd() + "\\transcad.log"
        if TransCAD.__dkInstance is None:
            print("Connecting to TransCAD...")
            TransCAD.__dkInstance = Gisdk(application_name="TransCAD", log_file=log_file)
        return TransCAD.__dkInstance

    @staticmethod
    def disconnect() -> bool:
        """ Disconnect from TransCAD. Call it when you're done.
        """
        if not TransCAD.__dkInstance is None:
            TransCAD.__dkInstance.Close()
            TransCAD.__dkInstance = None
            print("Disconnected from TransCAD!")
            return True
        print("Was not connected to TransCAD!")
        return False


class Maptitude:

    """ Static instance of Maptitude Gisdk shared by all of my functions
    """
    __dkInstance = None

    @staticmethod
    def connect(log_file: str = None) -> Gisdk:
        """ Return the Maptitude Gisdk object. Can be called multiple times.
        """
        if log_file is None:
            log_file = os.getcwd() + "\\Maptitude.log"
        if Maptitude.__dkInstance is None:
            print("Connecting to Maptitude...")
            Maptitude.__dkInstance = Gisdk(application_name="Maptitude", log_file=log_file)
        return Maptitude.__dkInstance

    @staticmethod
    def disconnect() -> bool:
        """ Disconnect from Maptitude. Call it when you're done 
        """
        if not Maptitude.__dkInstance is None:
            Maptitude.__dkInstance.Close()
            Maptitude.__dkInstance = None
            print("Disconnected from Maptitude!")
            return True
        print("Was not connected to Maptitude!")
        return False


class TransModeler:

    """ Static instance of TransModeler Gisdk object shared by all of my functions
    """
    __dkInstance = None

    @staticmethod
    def connect(log_file: str = None) -> Gisdk:
        """ Return the TransModeler Gisdk object. Can be called multiple times 
        """
        if log_file is None:
            log_file = os.getcwd() + "\\TransModeler.log"
        if TransModeler.__dkInstance is None:
            print("Connecting to TransModeler...")
            TransModeler.__dkInstance = Gisdk(application_name="TransModeler", log_file=log_file)
        return TransModeler.__dkInstance

    @staticmethod
    def disconnect() -> bool:
        """ Disconnect from TransModeler when done. 
        """
        if not TransModeler.__dkInstance is None:
            TransModeler.__dkInstance.Close()
            TransModeler.__dkInstance = None
            print("Disconnected from TransModeler!")
            return True
        print("Was not connected to TransModeler!")
        return False


if __name__ == "__main__":
    print(""" Caliper Gisdk Core Module for Python 3 (c) 2023 Caliper Corporation, Newton MA, USA. All rights reserved.""")
