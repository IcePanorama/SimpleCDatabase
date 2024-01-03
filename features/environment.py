from behave import *
import os


def before_feature(context, feature):
    context.temp = "mydb.db"


def after_feature(context, feature):
    if os.path.exists(context.temp):
        os.remove(context.temp)
