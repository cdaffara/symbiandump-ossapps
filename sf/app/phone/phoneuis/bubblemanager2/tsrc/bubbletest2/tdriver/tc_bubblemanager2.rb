############################################################################
## 
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). 
## All rights reserved. 
## 
## This file is part of BubbleManager2. Reference TDriver test script. 
## 
## 
############################################################################

require 'matti'
require 'orbitbehaviours.rb'
include MattiVerify

require 'test/unit'

class TC_BubbleManager2 < Test::Unit::TestCase
  
  # setup and teardown methods are always executed before each and after each test case method
  def setup
    # get the calculator app running 
    @sut = MATTI.sut(:Id => "sut_qt")    
    @testapp = @sut.run(:name => "bubbletest2")
    
  end
  
  def teardown   
    @testapp.close        
  end
  
  def test_mobile_terminated_call
    
    # create incoming call
    @testapp.BubbleTestView.press_key(:kC) # ciphering
    @testapp.BubbleTestView.press_key(:kD) # divert    
    @testapp.BubbleTestView.press_key(:k1)
    
    verify( 5, "Incoming bubble shown" ) { @testapp.BubbleContainerWidget(:objectName => 'expandedBubble' )}
    # check heading
    verify { @testapp.BubbleHeadingWidget(:objectName => 'eb:bubbleHeading',
                                          :textLine1 => '+35850123456789',
                                          :textLine2 => 'calling',
                                          :indicator1 => 'qtg_mono_ciphering_off',
                                          :indicator2 => 'qtg_mono_call_diverted' ) }
    # check image
    verify { @testapp.BubbleImageWidget(:objectName => 'eb:callImage', 
                                        :imageName => 'qtg_large_avatar' ) }
                                        
    # answer the call
    @testapp.BubbleButton(:objectName => 'eb:leftButton').tap
    
    # check heading
    verify { @testapp.BubbleHeadingWidget(:objectName => 'eb:bubbleHeading',
                                          :textLine1 => '+35850123456789',
                                          :textLine2 => '',
                                          :indicator1 => 'qtg_mono_ciphering_off',
                                          :indicator2 => '' ) }
                                          
    # test mute
    verify_not { @testapp.HbLabel(:objectName => 'mutedIcon' ) }    
    @testapp.BubbleButton(:objectName => 'eb:leftButton').tap
    verify { @testapp.HbLabel(:objectName => 'mutedIcon' ) }
    @testapp.BubbleButton(:objectName => 'eb:leftButton').tap
    verify_not { @testapp.HbLabel(:objectName => 'mutedIcon' ) }
                                          
    # end the call
    @testapp.BubbleButton(:objectName => 'eb:rightButton').tap
    verify_not { @testapp.BubbleContainerWidget(:objectName => 'expandedBubble')}
                                                                                                                                                                                              
  end  
  
  def test_mobile_terminated_call_with_contact
    
    # create incoming call
    @testapp.BubbleTestView.press_key(:kI) # image
    @testapp.BubbleTestView.press_key(:kN) # name
    @testapp.BubbleTestView.press_key(:k1)
    
    verify( 5, "Incoming bubble shown" ) { @testapp.BubbleContainerWidget(:objectName => 'expandedBubble' )}
    # check heading
    verify { @testapp.BubbleHeadingWidget(:objectName => 'eb:bubbleHeading',
                                          :textLine1 => 'Gloria Andersson',
                                          :textLine2 => '+35850123456789',
                                          :textLine3 => 'calling',
                                          :indicator1 => '',
                                          :indicator2 => '' ) }
    # check image
    verify { @testapp.BubbleImageWidget(:objectName => 'eb:callImage', 
                                        :imageName => ':resources/contactpic.jpg' ) }
                                        
    # reject the call
    @testapp.BubbleButton(:objectName => 'eb:rightButton').tap
    
    verify_not { @testapp.BubbleContainerWidget(:objectName => 'expandedBubble')}
                                                                                                                                                                                              
  end  
  
  def test_waiting_call
    
    # create active call
    @testapp.BubbleTestView.press_key(:k1)
    @testapp.BubbleTestView.press_key(:k3)
    
    # waiting call
    @testapp.BubbleTestView.press_key(:k1)
    
    verify( 5, "Active bubble shown" ) { @testapp.BubbleContainerWidget(:objectName => 'collapsedBubble' )}
    verify( 5, "Waiting bubble shown" ) { @testapp.BubbleContainerWidget(:objectName => 'expandedBubble' )}
    
    # check ongoing call heading
    verify { @testapp.BubbleHeadingWidget(:objectName => 'cb:bubbleHeading',
                                          :textLine1 => '+35850123456789',
                                          :textLine2 => '' ) }    
    
    # check waiting call heading
    verify { @testapp.BubbleHeadingWidget(:objectName => 'eb:bubbleHeading',
                                          :textLine1 => '+35840987654321',
                                          :textLine2 => 'waiting' ) }
                                          
    # check image
    verify { @testapp.BubbleImageWidget(:objectName => 'eb:callImage', 
                                        :imageName => 'qtg_large_avatar' ) }
                                        
                                        
    # answer the call
    @testapp.BubbleButton(:objectName => 'eb:leftButton').tap
    
    # check held call heading
    verify { @testapp.BubbleHeadingWidget(:objectName => 'cb:bubbleHeading',
                                          :textLine1 => '+35850123456789',
                                          :textLine2 => 'on hold' ) }     
    
    # check active call heading
    verify { @testapp.BubbleHeadingWidget(:objectName => 'eb:bubbleHeading',
                                          :textLine1 => '+35840987654321',
                                          :textLine2 => '' ) }                                            
    
  end  
  
  def test_conference_call
    
    # create conference call
    @testapp.BubbleTestView.press_key(:kT)
    @testapp.BubbleTestView.press_key(:k1)
    @testapp.BubbleTestView.press_key(:k3)
    @testapp.BubbleTestView.press_key(:k1)
    @testapp.BubbleTestView.press_key(:k3)
    @testapp.BubbleTestView.press_key(:k7) # join
    
    verify( 5, "Active bubble shown" ) { @testapp.BubbleContainerWidget(:objectName => 'expandedBubble' )}
    verify { @testapp.BubbleHeadingWidget(:objectName => 'eb:bubbleHeading',
                                          :textLine1 => 'Conference call',
                                          :textLine2 => '00:00:00' ) }    
                                          
    # check that calls are present in participant list
    verify { @testapp.HbGroupBox(:heading => '+35850123456789' ) }
    verify { @testapp.HbGroupBox(:heading => '+35840987654321' ) }

    # reject the call
    @testapp.BubbleButton(:objectName => 'eb:rightButton').tap    
    verify_not { @testapp.BubbleContainerWidget(:objectName => 'expandedBubble')}
                                                
  end
  
end
