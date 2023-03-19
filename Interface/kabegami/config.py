APP_ID = "@APP_ID@".removesuffix('.Devel')
APP_VERSION = "@APP_VERSION@"
APP_NAME = "@APP_NAME@"

APP_NAME_LOWER = APP_NAME.lower()
RESOURCES = '/' + APP_ID.removesuffix('.Devel').replace('.', '/')
APP_ICON = APP_ID

BUILD_TYPE = "@BUILD_TYPE@"