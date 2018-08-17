@ECHO OFF

IF %PROCESSOR_ARCHITECTURE%==x86 (
  SET APP_ROOT=%ADPROOT%\examples_perseus6010\Perseus6010_mi125_record_playback\host\prj_win\Release
) ELSE ( 
  SET APP_ROOT=%ADPROOT%\examples_perseus6010\Perseus6010_mi125_record_playback\host\prj_win\x64\Release
)

REM Record file name
SET REC_FILENAME=%ADPROOT%\examples_perseus6010\Perseus6010_mi125_record_playback\bin\record.bin
REM Perseus ip address
SET IP_ADDR_PERSEUS=192.168.0.101
REM 16 channels mode (bottom)
SET CHANNELSMODE=1
REM Internal 125MHz clk source
SET CLKSRC=0
REM Internal software trigger
SET TRIGSRC=0
REM Record size in number of samples per channel
SET SAMPLESPERCHANNEL=65536

%APP_ROOT%\perseus6010_mi125_acquisition_rtdex.exe %CHANNELSMODE% %TRIGSRC% %REC_FILENAME% %IP_ADDR_PERSEUS% %CLKSRC% %SAMPLESPERCHANNEL%
