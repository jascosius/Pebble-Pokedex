var first_lang = 0;
var second_lang = -1;

Pebble.addEventListener("ready", function() {
  console.log("Ready Event");
  
  first_lang = localStorage.getItem("first_lang");
  if (!first_lang) {
    first_lang = 0; // Default: English
  }

  second_lang = localStorage.getItem("second_lang");
  if (!second_lang) {
    second_lang = -1; // Default: None
  }

});

Pebble.addEventListener("showConfiguration", function(e) {
  console.log("showConfiguration Event");
            
  Pebble.openURL("https://www.jascosius.de/pebble/pokedex/pokedex.php?" +
           "first_lang=" + first_lang +
           "&second_lang=" + second_lang);
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("Configuration window closed");
  console.log(e.type);
  console.log(e.response);

  var configuration = JSON.parse(e.response);
  Pebble.sendAppMessage(configuration);
  
  first_lang = configuration["first_lang"];
  localStorage.setItem("first_lang", first_lang);
  
  second_lang = configuration["second_lang"];
  localStorage.setItem("second_lang", second_lang);
});
