import os
import importlib.machinery

modules = [0]

channel = "./.pychannel"
lock = "./.pylock"

def read_command(name : str):
    with open(name, "r") as r:
        data = r.readlines()
        if(len(data) == 0):
            return ['NULL', 'NULL']
        elif(len(data) == 1):
            return [data[0].rstrip("\n"), 'NULL']
        else:
            return [data[0].rstrip("\n"), data[1].rstrip("\n")]
    
def push_command(command : str, value : str, name : str):
    with open(name, "w") as w:
        w.write(command)
        w.write("\n")
        w.write(value)
        w.close()
    return

def create(name):
    with open(name, "w") as w:
        w.close()

while 1:
    try:
        if(os.path.exists(lock)):
            cmd = read_command(channel)
            if cmd[0] == "kill":
                os.remove(channel)
                os.remove(lock)
                break
            
            elif cmd[0] == "check_running":
                push_command("check_running", "true", channel)
                os.remove(lock)
            elif cmd[0] == "import":
                try:
                    modules.append(importlib.machinery.SourceFileLoader("imported", cmd[1]).load_module())
                    push_command('import_response', str(len(modules) - 1), channel)
                    os.remove(lock)
                except Exception as e:
                    push_command("import_response", "fail", channel)
                    os.remove(lock)
            elif cmd[0] == "call":
                try:
                    # name:arg1;;arg2;;'arg3':4
                    full = cmd[1].split(":")
                    name = full[0]
                    args = tuple(full[1].split(';;'))
                    module = int(full[2])
                    if module == 0:
                        push_command("response", "FAIL", channel)
                        os.remove(lock)
                        continue
                    fnc = getattr(modules[module], name)
                    out = str(fnc(*args))
                    push_command("response", out, channel)
                    os.remove(lock)

                except Exception as e:
                    push_command("exception", str(e), channel)
                    os.remove(lock)
    except:
        continue