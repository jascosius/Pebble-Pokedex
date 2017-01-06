<!DOCTYPE html>
<html>
  <head>
    <title>Pokedex</title>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='stylesheet' href='jquery.mobile-1.4.5.min.css' />
    <script src='jquery-1.11.3.min.js'></script>
    <script src='jquery.mobile-1.4.5.min.js'></script>
    <style>
      .ui-header .ui-title { margin-left: 1em; margin-right: 1em; text-overflow: clip; }
    </style>
  </head>
  <body>
    <div data-role="page" id="page1">
      <div data-theme="a" data-role="header" data-position="fixed">
      <h3>
        Pokedex Configuration
      </h3>
      <div class="ui-grid-a">
        <div class="ui-block-a">
          <input id="cancel" type="submit" data-theme="c" data-icon="delete" data-iconpos="left" value="Cancel" data-mini="true">
        </div>
        <div class="ui-block-b">
          <input id="save" type="submit" data-theme="b" data-icon="check" data-iconpos="right" value="Save" data-mini="true">
        </div>
      </div>
    </div>

    <div data-role="content">
      <div id="first_lang" data-role="fieldcontain">
        <fieldset data-role="controlgroup" data-type="horizontal" data-mini="true">
          <legend>First language</legend>

          <?php
            if (!isset($_GET['first_lang'])) {
              $first_lang = 0;
            } else {
              $first_lang = $_GET['first_lang'];
            }

            $lang_eng = "";
            $lang_ger = "";
            $lang_eng_disabled = "";
            $lang_ger_disabled = "";
	
            if ($first_lang == 0) {
              $lang_eng = " checked";
              $lang_eng_disabled = " disabled";
            } else {
              $lang_ger = " checked";
              $lang_ger_disabled = " disabled";
            }
	
            echo '<input id="first_lang_eng" name="first_lang" value="0" onclick="handleClick(this);" data-theme="" type="radio"' . $lang_eng . '><label for="first_lang_eng">English</label>';
            echo '<input id="first_lang_ger" name="first_lang" value="1" onclick="handleClick(this);" data-theme="" type="radio"' . $lang_ger . '><label for="first_lang_ger">German</label>';
          ?>
         </fieldset>
      </div>

      <div id="second_lang" data-role="fieldcontain">
        <fieldset data-role="controlgroup" data-type="horizontal" data-mini="true">
          <legend>Second language</legend>

          <?php
            if (!isset($_GET['second_lang'])) {
              $second_lang = -1;
            } else {
              $second_lang = $_GET['second_lang'];
            }

            $lang_eng2 = "";
            $lang_ger2 = "";
            $lang_none2 = "";
	
            if ($second_lang == 0) {
              $lang_eng2 = " checked";
            } elseif ($second_lang == 1) {
              $lang_ger2 = " checked";
            } else {
              $lang_none2 = " checked";
            }
	
            echo '<input id="second_lang_eng" name="second_lang" value="0" data-theme="" type="radio"' . $lang_eng2 . '><label for="second_lang_eng">English</label>';
            echo '<input id="second_lang_ger" name="second_lang" value="1" data-theme="" type="radio"' . $lang_ger2 . '><label for="second_lang_ger">German</label>';
            echo '<input id="second_lang_none" name="second_lang" value="-1" data-theme="" type="radio"' . $lang_none2 . '><label for="second_lang_none">None</label>';
	      ?>
        </fieldset>
      </div>
    </div>

    <script>
      function handleClick(myRadio) {
        if(myRadio.id == 'first_lang_eng') {
          $('#second_lang_ger').prop('disabled', false);
          if($('#second_lang_eng').is(':checked')) {
            $('#second_lang_eng').prop('checked', false);
            $('#second_lang_ger').prop('checked', true);
          }
          $('#second_lang_eng').prop('disabled', true);
        }
        if(myRadio.id == 'first_lang_ger') {
          $('#second_lang_eng').prop('disabled', false);
          if($('#second_lang_ger').is(':checked')) {
            $('#second_lang_ger').prop('checked', false);
            $('#second_lang_eng').prop('checked', true);
          }
          $('#second_lang_ger').prop('disabled', true);
        }
        $("[type=checkbox]").checkboxradio();
        $("[data-role=controlgroup]").controlgroup("refresh");
      }

      function saveOptions() {
        var options = {
          'first_lang': parseInt($("input[name=first_lang]:checked").val(), 10),
          'second_lang': parseInt($("input[name=second_lang]:checked").val(), 10)
        }
        return options;
      }

      $().ready(function() {
        console.log("Hallo");
        if($('#first_lang_eng').is(':checked')) {
          $('#second_lang_eng').prop('disabled', true);
        }
        if($('#first_lang_ger').is(':checked')) {
          $('#second_lang_ger').prop('disabled', true);
        }
        $("[type=checkbox]").checkboxradio();
        $("[data-role=controlgroup]").controlgroup("refresh");
        $("#cancel").click(function() {
          console.log("Cancel");
          document.location = "pebblejs://close#";
        });

        $("#save").click(function() {
          console.log("Submit");
          var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
          console.log("Close: " + location);
          console.log(location);
          document.location = location;
        });

      });
    </script>
  </body>
</html>
