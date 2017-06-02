# NexVM
A Virtual Machine Experimentation And Test Project

## Introduction
I worked on this project a few years ago, as a forray into my interest of virtual machines during high school. 
Although the code itself is most likely not that interesting, as this is a early test revision to develop
the interpreter, I thought I would push the project to github for hecks sake. 

## Why Don't You Post The Last Revision
The last revision which was stored on my local NAS and subsequently lost when I accidently was screwing with FDISK :), 
if I find it, I will push it to Github.

## What Does This Revision Feature 
"Not Much", this revision features mostly the beginnings of 
  1. The interpreter, which utilizes ugly "computed gotos" (requiring LLVM or GCC), reduces overhead by not filling 
  and operating on the call stack.
  2. The object model, is partially implemented with computed metadata.
  3. The classloading and dispatch system is also partially implemented
  4. The beginnings of the native invocation system
  
## What Does This Revision Not Feature
  1. The LLVM IR generation frontend is not at all implemented in this revision
  2. The later more efficent object model, which reduced overhead via storing without STL and enumerated strings as constants internally
  3. Calling external libraries is not supported (except the hardwired STDL.a, used for testing)
  4. Dead code optomization precompiler, which did some inlining and removed dead code.

## Will You Write A Virtual Machine In The Future
Probabally not, I am not an expert in code analysis and optomization, nor do I wish to become one. Although I may implement a lisp 1 machine or something if I'm bored.
  
  
