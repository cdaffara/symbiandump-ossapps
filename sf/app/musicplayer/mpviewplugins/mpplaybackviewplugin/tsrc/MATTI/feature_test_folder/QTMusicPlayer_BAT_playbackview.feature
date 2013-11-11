Feature: As a test engineer I want to run BAT test cases for QT Music Player
  
  Scenario: Equalizer_Select a preset from list
    Given the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    When I Tap on Navi pane text area Music
    When I select Equaliser from the Options menu
    Then the Equaliser pop up is displayed
    And Equaliser preset 1 is selected
    When I select the Equaliser preset number 3 from the list
    And I Tap on Navi pane text area Music
    And I select Exit from the Options menu
    Then the MusicPlayer application is not active
    When the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    When I Tap on Navi pane text area Music
    When I select Equaliser from the Options menu
    Then the Equaliser preset 3 is selected
    
  Scenario: Repeat
    Given the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    And Repeat is set to ON
    Then all songs are played and at the end it goes to the first one and stays in Playing
    When I Tap on Navi pane text area Music
    And I select Repeat off from the Options menu
    And all songs are played and at the end it goes to the first one and stays in Paused
    When the application is closed
    Given the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    And Repeat is set to OFF
    

    
    
    
    
    
    
    
    
    
  Scenario: Audio Effects view - Change settings and Exit
    Given the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    When I Tap on Navi pane text area Music
    When I select Audio effects from the Options menu
    Then the Audio effects pop up is displayed
    And Audio effects settings are Balance 0 and Loudness Off
    When I drag the slider to the Right 40 pixels
    And I tap the Loudness toggle
    Then Audio effects settings are Balance 25 and Loudness On
    When the application is closed
    Given the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    When I Tap on Navi pane text area Music
    When I select Audio effects from the Options menu
    Then the Audio effects pop up is displayed
    And Audio effects settings are Balance 25 and Loudness On
    
  Scenario: Playbackview options menu
    Given the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    When I Tap on Navi pane text area Music
    Then the playback view options menu list is shown correctly
    
  Scenario: Now Playing View Q4- Activate shuffle- go back to Songs view and play another song - check shuffle is ON
		Given the MusicPlayer application is active
    And I select the song number 1 of the list
    Then The song number 1 of the library is shown in playback view
    When I set shuffle to ON
    Then musicplayer plays the following songs after song 1 randomly
    When I press the back button
    And I select the song number 4 of the list
    Then The song number 4 of the library is shown in playback view
    And shuffle is set to ON
    
  Scenario: Now playing view H1- Shuffle OFF
    Given the MusicPlayer application is active
    And I select the song number 1 of the list 
    Then The song number 1 of the library is shown in playback view
    When I set shuffle to OFF
    And I wait until the song change
    Then The song number 2 of the library is shown in playback view
    And I wait until the song change
    Then The song number 3 of the library is shown in playback view
    Then the application is closed
    
  Scenario: Playback view control bar to FW,RW,Play-pause.
    Given the MusicPlayer application is active
    And I select the song number 1 of the list 
    Then The song number 1 of the library is shown in playback view
    And the song in the playback view is Playing
    When I press the button FW in the PlaybackToolbar
    Then The song number 2 of the library is shown in playback view
    And the song in the playback view is Playing
    When I press the button RW in the PlaybackToolbar twice
    Then The song number 1 of the library is shown in playback view
    When I single press the button RW in the PlaybackToolbar and it is playing it starts from the beginning 
    When I press the button Pause in the PlaybackToolbar
    Then the song in the playback view is Paused
    When I press the button FW in the PlaybackToolbar
    Then The song number 2 of the library is shown in playback view
    Then the song in the playback view is Paused
    When I press the button RW in the PlaybackToolbar
    Then The song number 1 of the library is shown in playback view
    Then the song in the playback view is Paused