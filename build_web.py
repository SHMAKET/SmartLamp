Import("env")
import subprocess

def build_web(source, target, env):
    subprocess.call(["npm", "run", "build"])

env.AddPreAction("buildfs", build_web)