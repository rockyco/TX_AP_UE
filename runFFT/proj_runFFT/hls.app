<project xmlns="com.autoesl.autopilot.project" name="proj_runFFT" top="runFFT" projectType="C/C++">
    <includePaths/>
    <libraryPaths/>
    <Simulation>
        <SimFlow name="csim" csimMode="0" lastCsimMode="0"/>
    </Simulation>
    <files xmlns="">
        <file name="../../goldenFFT.txt" sc="0" tb="1" cflags="-Wno-unknown-pragmas" csimflags="" blackbox="false"/>
        <file name="../../data4FFT.txt" sc="0" tb="1" cflags="-Wno-unknown-pragmas" csimflags="" blackbox="false"/>
        <file name="../../runFFT_tb.cpp" sc="0" tb="1" cflags="-Wno-unknown-pragmas" csimflags="" blackbox="false"/>
        <file name="fft_top.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
        <file name="runFFT.cpp" sc="0" tb="false" cflags="" csimflags="" blackbox="false"/>
    </files>
    <solutions xmlns="">
        <solution name="solution1" status="active"/>
    </solutions>
</project>

