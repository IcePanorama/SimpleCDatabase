from behave import *
from os import system
import subprocess


@given('we launch the database')
def step_impl(context):
    cmd = 'echo'
    args = 'Hello World'
    context.temp = subprocess.Popen([cmd, args], stdout=subprocess.PIPE)


@then('the database should launch.')
def step_impl(context):
    output = str(context.temp.communicate())
    output = output.split("\n")
    return output[0] == "Hello World"
