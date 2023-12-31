from behave import *
import os


def before_scenario(context, scenario):
    context.temp = os.popen('rm mydb.db')


def after_scenario(context, scenario):
    context.temp = os.popen('rm mydb.db')
