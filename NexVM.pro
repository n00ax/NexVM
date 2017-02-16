TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    NexVM/interpreter/BaseInterpreter.cpp \
    NexVM/interpreter/InterpreterTestBench.cpp \
    NexVM/allocator/Allocator.cpp \
    NexVM/exception/ExceptionHandeler.cpp \
    NexVM/exception/Logger.cpp \
    NexVM/object/NexObject.cpp \
    NexVM/classloader/ClassLoader.cpp \
    NexVM/platform/PosixPlatform.cpp \
    NexVM/initialization/VMInit.cpp \
    NexVM/native/NativeLoader.cpp

HEADERS += \
    NexVM/interpreter/BaseInterpreter.h \
    NexVM/interpreter/InterpreterTestBench.h \
    NexVM/interpreter/BuildConstants.h \
    NexVM/allocator/Allocator.h \
    NexVM/object/ClassDefinitions.h \
    NexVM/exception/ExceptionHandeler.h \
    NexVM/exception/Logger.h \
    NexVM/object/NexObject.h \
    NexVM/classloader/ClassLoader.h \
    NexVM/platform/Platform.h \
    NexVM/initialization/VMInit.h \
    NexVM/native/NativeLoader.h
