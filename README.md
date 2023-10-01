
# Description

Point of including lwip sourses to project instead of include lib from mss is to configure some custom settings without modifing make and lib files.


## How to build a project

For building a project, you have to use the [GNU Make tool](https://www.gnu.org/software/make/). If you're a 
Windows user please checkout [this page](https://wiki.analog.com/resources/fpga/docs/build#windows_environment_setup), to see how you can install this tool.

To build a project just **cd** to the project and run make:
```
 [~]cd projects/lan/kcu_105
 [~]make
```

## Software

1. After building the project, run Vivado -> open project -> Launch SDK (exported location - /sw_src; workspace - /sw_src)

2. In SDK file/new/project -> specify name, next -> empty application -> finish

3. Аdd sw_src/src files to your project.


