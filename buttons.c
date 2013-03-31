#include "device.h"
#include "display.h"
#include "buttons.h"
#include "interrupts.h"
void Buttons(Button button)  
{
  switch(button)
  {
  case volumeUp:  //if volumeUp button pressed
    sbState.nVolume++;
    if (sbState.nVolume > 15) sbState.nVolume = 15;
    if (sbState.nVolume <= 0) sbState.nVolume = 0;
    SetVolume(sbState.nVolume);
    break;
  
  case volumeDown: //if volumeDown button pressed
    sbState.nVolume--;
    if (sbState.nVolume > 15) sbState.nVolume = 15;
    if (sbState.nVolume <= 0) sbState.nVolume = 0;
    SetVolume(sbState.nVolume);
    break;
  
  case usbMode:   //if USB button pressed
    PCM3070_USB_IN();          
    break;
    
  case spdifMode: //if SPDIF button pressed 
    PCM3070_SPDIF_IN();                
    break;
    
  case analogMode:  //if Analog button pressed
    if (sbState.mode == analog)
      sbState.nInput++;
    sbState.mode = analog;
    if ((sbState.nInput > 3) || (sbState.nInput < 1))
      sbState.nInput = 1;
    SetAnalogMode(sbState.nInput);                
    break;
    
  case muteMode:  //if MUTE button pressed
    sbState.bMute = !sbState.bMute;
    SetMute(sbState.bMute);                
    break;
    
  case shutdownMode:  // if SHUTDOWN button pressed 
    sbState.bShutdown = !sbState.bShutdown;
    SetShutdown(sbState.bShutdown);
    break; 
    
  case surround0:     //If Surround0 button pressed
    if (sbState.nSurround & SURROUND_0)
      sbState.nSurround &= ~SURROUND_0;
    else
      sbState.nSurround |= SURROUND_0;
    SetSurround(sbState.nSurround);
    break;
    
  case surround1:    //if Surround1 button pressed 
    if (sbState.nSurround & SURROUND_1)
      sbState.nSurround &= ~SURROUND_1;
    else
      sbState.nSurround |= SURROUND_1;
    SetSurround(sbState.nSurround);
    break;
    
  default:
    break;
  }
  
  StartDisplayTimer();
  UpdateDisplay();
}
