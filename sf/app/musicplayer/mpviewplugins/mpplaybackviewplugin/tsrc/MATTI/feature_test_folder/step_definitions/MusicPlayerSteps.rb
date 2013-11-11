def mda(width,height)
a = Array.new(width)
a.map! { Array.new(height) }
return a
end

def create_mp_constants
@Library = mda(4,3)
@Library[0] = ["Amen and Boys", "Stop It!!","Self Made Maps"]
@Library[1] = ["Emancipated Minor", "Ani Difranco","Red Letter Year"]
@Library[2] = ["Slowly", "Fauna","Dreamless"]
@Library[3] = ["Turkish March", "Daniel Bautista","Classics And Soundtracks"]

@PlaybackToolBarOrder = {"Shuffle"=>0,"RW"=>1,"Pause"=>2,"Play"=>2,"FW"=>3,"Flip"=>4}

@PlaybackViewOptionsMenu = {"Equaliser"=>0, "Audio effects"=>1, "Repeat off" =>2, "Exit" => 3}

@EquiliserPresetsList = {"Off"=>0, "Bass booster"=>1, "Classical" =>2, "Jazz" => 3, "Pop"=>4 , "Rock"=> 5}

@ShuffleOffIconConst = 'qtg_mono_shuffle_off'
@ShuffleOnIconConst = 'qtg_mono_shuffle'
@RWIconConst = 'qtg_mono_seek_previous'
@PauseIconConst = 'qtg_mono_pause'
@FWIconConst = 'qtg_mono_seek_next'
@InfoIconConst = 'qtg_mono_info'
@PlayIconConst  = 'qtg_mono_play'


end

require 'matti'
require 'spec/expectations'
require 'nokogiri'
require 'date'

Before do
  create_mp_constants
  @sut = MATTI.sut(:Id=>'sut_s60_qt')
end

After do
  begin
     @MPapp.close
  rescue
  end
end

Given("the MusicPlayer application is active") do 
  @MPapp = @sut.run(:name => "musicplayer")
end

Then("the MusicPlayer application is not active") do 
  verify_true(1, 'Music Player still running') {@sut.application.name != "musicplayer"}  #Verify that the app that is in the foreground is not music player, if it is that means musicplayer is still running.
end

When("I Tap on Navi pane text area Music") do 
    @MPapp.HbTitleBar.HbTitlePane.HbFrameItem.tap
    @MenuContainer = @MPapp.HbMenu.HbMenuContainer
end

When("I select the song number $element_num of the list") do |element_num|
  @MPapp.HbListView(:name=>'allSongsList').HbListViewItem(:__index=>element_num.to_i-1).tap
end

Then("The song with the title $song_title and artist $song_artist and album $song_album is shown in playback view") do |song_title, song_artist, song_album|
  verify(10, "The song title is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'songText').HbTextItem(:text => song_title)}
  verify(10, "The song artist is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'artistText').HbTextItem( :text => song_artist)}
  verify(10, "The song album is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'albumText').HbTextItem( :text => song_album)}
end

Then("The song number $num of the library is shown in playback view") do |num|
  verify(10, "The song title is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'songText').HbTextItem(:text => @Library[num.to_i-1][0])}
  verify(10, "The song artist is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'artistText').HbTextItem( :text => @Library[num.to_i-1][1])}
  verify(10, "The song album is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'albumText').HbTextItem( :text => @Library[num.to_i-1][2])}
end

Then ("musicplayer plays the following songs after song $num randomly") do |num|
  @MPapp.MpPlaybackWidget.wait_for_signal(600, "signalPlaybackInfoChanged()")
  title=@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'songText').HbTextItem.attribute("Text")
  artist=@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'artistText').HbTextItem.attribute("Text")
  album=@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'albumText').HbTextItem.attribute("Text")
  if title != @Library[num.to_i][0] or artist != @Library[num.to_i][1] or album != @Library[num.to_i][2]
  else
    @MPapp.MpPlaybackWidget.wait_for_signal(600, "signalPlaybackInfoChanged()")
    verify_not(10, "The song title is being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'songText').HbTextItem(:text => @Library[num.to_i+1][0])}
    verify_not(10, "The song artist is being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'artistText').HbTextItem( :text => @Library[num.to_i+1][1])}
    verify_not(10, "The song album is being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'albumText').HbTextItem( :text => @Library[num.to_i+1][2])}
  end
end

Then("the song in the playback view is $playback_status") do |playback_status|
    verify(5, "The status of the playback view is not the spected"){@MPapp.MpPlaybackView(:PlaybackStatus => playback_status)}
    lb_init = @MPapp.HbProgressSlider( :name => 'progressBar' ).HbTextItem(:__index => 0).attribute("Text")
    sleep 5
    lb_fin = @MPapp.HbProgressSlider( :name => 'progressBar' ).HbTextItem(:__index => 0).attribute("Text")
    if playback_status == 'Playing'
      verify_true(1,"Time label didn't change") {lb_init != lb_fin}
    end
    if playback_status == 'Paused'
      verify_true(1,"Time label didn't change") {lb_init == lb_fin}
    end
end

When ("I press the button $button in the PlaybackToolbar") do |button|
  verify_true(5,"Icon is not correct"){@MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch(button)).HbIconItem.attribute("iconName") == getPlaybackToolbarIconName(button)}
  @MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch(button)).tap
end

When ("I sleep $number $min_sec") do |number, min_sec|
  time_to_wait = 0
  if(min_sec == "seconds")
    time_to_wait = number.to_i
  end
  if(min_sec == "minutes")
    time_to_wait = number.to_i*60
  end
  puts time_to_wait.to_s
  sleep time_to_wait
end

When ("I wait until the song change") do
  @MPapp.MpPlaybackWidget.wait_for_signal(600, "signalPlaybackInfoChanged()")
end

def to_true_false(param)
  if param == true or param == "ON"
    return "true"
  end
  if param == false or param == "OFF"
     return "false"
  end
end
      
When ("I set shuffle to $on_off") do |on_off|
    if @MPapp.MpPlaybackView.attribute("Shuffle") != to_true_false(on_off)
      @MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch("Shuffle")).tap
    end
    verify_true(2,"Shuffle is not the expected value"){@MPapp.MpPlaybackView.attribute("Shuffle") == to_true_false(on_off)}
    if on_off == "ON"
      verify_true(5,"Shuffle icon is not correct"){@MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch("Shuffle")).HbIconItem.attribute("iconName") == @ShuffleOnIconConst}
    end
    if on_off == "OFF"
      verify_true(5,"Shuffle icon is not correct"){@MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch("Shuffle")).HbIconItem.attribute("iconName") == @ShuffleOffIconConst}
    end
    
end

When("shuffle is set to $on_off") do |on_off|
  verify_true(3,"Shuffle is not the expected value"){@MPapp.MpPlaybackView.attribute("Shuffle") == to_true_false(on_off)}
  if on_off == "ON"
    verify_true(5,"Shuffle icon is not correct"){@MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch("Shuffle")).HbIconItem.attribute("iconName") == @ShuffleOnIconConst}
  end
  if on_off == "OFF"
    verify_true(5,"Shuffle icon is not correct"){@MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch("Shuffle")).HbIconItem.attribute("iconName") == @ShuffleOffIconConst}
  end
end

When ("I press the button $button in the PlaybackToolbar twice") do |button|
  @MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch(button)).tap(2)
end

When("I press the back button") do
  @MPapp.HbTitleBar.HbNavigationButton.tap
end

When ("I single press the button RW in the PlaybackToolbar and it is playing it starts from the beginning") do
  lb_init = @MPapp.HbProgressSlider( :name => 'progressBar' ).HbTextItem(:__index => 0).attribute("Text")
  @MPapp.HbToolBar( :name => 'PlaybackToolbar' ).HbToolButton(:__index => @PlaybackToolBarOrder.fetch("RW")).tap
  lb_fin = @MPapp.HbProgressSlider( :name => 'progressBar' ).HbTextItem(:__index => 0).attribute("Text")
  verify_true(1,"It didn't start from the beginning"){lb_init > lb_fin}
end

When("I select $item from the Options menu") do |item|
  @MenuContainer.HbMenuItem(:__index=> @PlaybackViewOptionsMenu.fetch(item)).tap
end

Then ("the playback view options menu list is shown correctly") do
  i = 0
  while i <  @PlaybackViewOptionsMenu.length do
      verify_true(2,"Menu item is incorrect"){@MenuContainer.HbMenuItem(:__index=> i).HbTextItem.attribute("Text") == @PlaybackViewOptionsMenu.index(i)} 
      i = i +1
  end
  
 # @PlaybackViewOptionsMenu.each{|key, value| verify_true(2,"Menu item is incorrect"){@MenuContainer.HbMenuItem(:__index=> value).HbTextItem.attribute("Text") == key}  }
end

Then ("the Equaliser pop up is displayed") do
  verify(3, "Equiliser pop up is not displayed"){@MPapp.MpEqualizerWidget}
  i = 0
  while i < @EquiliserPresetsList.length  do
    puts i.to_s
    puts @MPapp.MpEqualizerWidget.HbRadioButtonList.HbListViewItem(:__index=> i).HbTextItem.attribute("Text")
    puts @EquiliserPresetsList.index(i)
    verify_true(2,"Menu item is incorrect"){@MPapp.MpEqualizerWidget.HbRadioButtonList.HbListViewItem(:__index=> i).HbTextItem.attribute("Text") == @EquiliserPresetsList.index(i)}
    i = i+1
  end
end

Then ("Equaliser preset $num is selected") do |num|
  
  verify_true(3, "Item is not selected"){@MPapp.MpEqualizerWidget.HbRadioButtonList.HbListViewItem(:__index=> num.to_i-1).HbIconItem.attribute("iconName") == "qtg_small_radio_selected"}
end

When ("I select the Equaliser preset number $num from the list") do |num|
  @MPapp.MpEqualizerWidget.HbRadioButtonList.HbListViewItem(:__index=>num.to_i - 1).tap
  @MPapp.MpEqualizerWidget.HbToolBar.HbTextItem( :text => 'OK' ).tap
end

Then ("the Audio effects pop up is displayed") do
  verify(3, "Audio effects pop up is not displayed"){@MPapp.MpSettingsAudioEffectsWidget}
  verify(3, "Balance slider is not present") {@MPapp.MpSettingsAudioEffectsWidget.HbSlider}
  verify(3, "Loudness toggle is not present"){@MPapp.MpSettingsAudioEffectsWidget.HbToggleItem}
end

Then ("Audio effects settings are Balance $balance_value and Loudness $loudness_value") do |balance_value, loudness_value|
  verify_true(3,"Balance value is not the expected"){@MPapp.MpSettingsAudioEffectsWidget.HbSlider.attribute("sliderPosition").to_i == balance_value.to_i}
  verify_true(3,"Loudness value is not the expected"){@MPapp.MpSettingsAudioEffectsWidget.HbToggleItem.attribute("Text") == loudness_value}
end

When("I drag the slider to the $dir $num_pix pixels") do |dir, num_pix|
  @MPapp.MpSettingsAudioEffectsWidget.HbSliderHandle.drag(to_direction(dir), num_pix, :Left)
end

When("the application is closed") do
  @MPapp.close
end

When("I tap the Loudness toggle") do
  @MPapp.MpSettingsAudioEffectsWidget.HbToggleItem.tap
end

When("Repeat is set to $on_off") do |on_off|
  verify_true(3,"Repeat is not the expected value"){@MPapp.MpPlaybackView.attribute("Repeat") == to_true_false(on_off)}
end

Then ("all songs are played and at the end it goes to the first one and stays in $playback_status") do |playback_status|
  i = 0
  while i < @Library.length  do
    verify(10, "The song title is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'songText').HbTextItem(:text => @Library[i][0])}
    verify(10, "The song artist is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'artistText').HbTextItem( :text => @Library[i][1])}
    verify(10, "The song album is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'albumText').HbTextItem( :text => @Library[i][2])}
    
    @MPapp.MpPlaybackWidget.wait_for_signal(720, "signalPlaybackInfoChanged()")
    i = i+1
  end
  verify(10, "The song title is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'songText').HbTextItem(:text => @Library[0][0])}
  verify(10, "The song artist is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'artistText').HbTextItem( :text => @Library[0][1])}
  verify(10, "The song album is not being displayed") {@MPapp.HbLabel(:visibleOnScreen => 'true', :name => 'albumText').HbTextItem( :text => @Library[0][2])}
  verify_true(5, "The status of the playback view is not the spected"){@MPapp.MpPlaybackView.attribute("PlaybackStatus") == playback_status or @MPapp.MpPlaybackView.attribute("PlaybackStatus") == "Stopped"}  
end

def to_direction(string)
    case string
      when "Right"
        return :Right
      when "Left"
        return :Left
      when "Up"
        return :Up
      when "Down"
        return :Down
    end
end

def getPlaybackToolbarIconName(button)
    case button
      when "RW"
        return @RWIconConst
        when "Pause"
        return @PauseIconConst
        when "FW"
        return @FWIconConst
        when "Info"
        return @InfoIconConst
        when "Play"
        return @PlayIconConst
      end
      
end
  


