%-
%- RNet source files
%-
%FILE %'DirRel_Code'LICENSE_RNet.txt
%include RNet\LICENSE_RNet.txt

%FILE %'DirRel_Code'README_RNet.txt
%include RNet\LICENSE_RNet.txt

%FILE %'DirRel_Code'Radio.h
%include RNet\Radio.h

%FILE %'DirRel_Code'RApp.c
%include RNet\RApp.c

%FILE %'DirRel_Code'RApp.h
%include RNet\RApp.h

%FILE %'DirRel_Code'RMAC.c
%include RNet\RMAC.c

%FILE %'DirRel_Code'RMAC.h
%include RNet\RMAC.h

%FILE %'DirRel_Code'RMSG.c
%include RNet\RMSG.c

%FILE %'DirRel_Code'RMSG.h
%include RNet\RMSG.h

%FILE %'DirRel_Code'RNetConf.h
%include RNet\RNetConf.h

%FILE %'DirRel_Code'RNWK.c
%include RNet\RNWK.c

%FILE %'DirRel_Code'RNWK.h
%include RNet\RNWK.h

%FILE %'DirRel_Code'RPHY.c
%include RNet\RPHY.c

%FILE %'DirRel_Code'RPHY.h
%include RNet\RPHY.h

%FILE %'DirRel_Code'RStack.c
%include RNet\RStack.c

%FILE %'DirRel_Code'RStack.h
%include RNet\RStack.h

%if defined(RStdioEnabled) & %RStdioEnabled='yes'
  %FILE %'DirRel_Code'RStdIO.c
  %include RNet\RStdIO.c

  %FILE %'DirRel_Code'RStdIO.h
  %include RNet\RStdIO.h
%endif

%if TransceiverType='RNET_CONFIG_TRANSCEIVER_NRF24'
  %FILE? %'DirRel_Code'Radio.c
  %include RNet\nRF24\Radio.c
  
  %FILE? %'DirRel_Code'RadioNRF24.h
  %include RNet\nRF24\RadioNRF24.h
%elif TransceiverType='RNET_CONFIG_TRANSCEIVER_MC13201'
  %FILE? %'DirRel_Code'Radio.c
  %include RNet\SMAC\Radio.c
  
  %FILE? %'DirRel_Code'RadioSMAC.h
  %include RNet\SMAC\RadioSMAC.h
%else
 %error unsupported RNet transceiver type '%TransceiverType'
%endif
