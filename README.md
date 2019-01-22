<h1 align="center">SIMD Histogram</h1>
<h5 align="center">Multithreaded, vectorized tool for calculating image histograms (created while learning the basics of MASM)</h5>

![Application Screenshot](https://github.com/96wysocki/simd-histogram/blob/master/doc/screenshot.png)

#### Project structure
```c
+-- 
   +-- DLL_ASM  // histogram calculations library in assembly language
      +-- 
        ...          
   +-- DLL_CPP  // histogram calculations library in C++
      +--
        ...
   +-- MAIN     // main app (Qt)
      +-- 
       ...
   +-- 
    ...
```

#### Built with
- [Qt](http://doc.qt.io)
- [QtCharts](https://doc.qt.io/qt-5/qtcharts-index.html)
- [Microsoft Macro Assembler x64](https://docs.microsoft.com/en-us/cpp/assembler/masm)
