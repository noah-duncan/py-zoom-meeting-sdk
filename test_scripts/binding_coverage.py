import nanobind as nb
import inspect
import zoom_meeting_sdk_python as zoom

if __name__ == "__main__":
    bound_classes = [name for name, obj in inspect.getmembers(zoom) if type(obj).__name__ == 'nb_type_0']
    bound_functions = [name for name, obj in inspect.getmembers(zoom) if type(obj).__name__ == 'nb_func']

    print('bound_classes =', bound_classes)
    print('bound_functions =', bound_functions)