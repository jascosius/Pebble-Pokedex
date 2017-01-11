#Quick and dirty script to select and print all needed information in the nessesary format

$generations = [001,152,252,387,494,650,722]
$pokemonCount = 801

#Copy of the table
#http://www.pokewiki.de/Pok%C3%A9mon-Liste
$filePokemon = "pokewiki_pokemon.txt"

#Copy of the table
#http://www.pokewiki.de/Rangliste_aller_Legend%C3%A4ren_Pok%C3%A9mon
$fileLegendary = "pokewiki_legendary.txt"

#Copy of the table
#http://www.pokewiki.de/Liste_aller_Mega-Pok%C3%A9mon
$fileMega = "pokewiki_mega.txt"

#Copy of the table
#http://www.pokewiki.de/Regionalform
$fileAlola = "pokewiki_alola.txt"

#Source of the page
##http://www.pokewiki.de/Pok%C3%A9mon-Liste
$fileHtml = "pokewiki_pokemon.html.txt"

$imageSize = [30,27]
$pokemonPerImage = 9
$threshold = 50 # %

$imagesRootPath = "http://www.pokewiki.de"
$imagesFolder = "images"

$pokemonList = []
$megaList = []
$alolaList = []

$pokemonTypes = {none: 0, bug: 1, dark: 2, dragon: 3, electric: 4, fairy: 5, fighting: 6, fire: 7, flying: 8, ghost: 9, grass: 10, ground: 11, ice: 12, normal: 13, poison: 14, psychic: 15, rock: 16, steel: 17, water: 18}

$pokemonTypesString = {nil => :none, "BodenIC.png" => :ground, "DracheIC.png" => :dragon, "EisIC.png" => :ice, "ElektroIC.png" => :electric, "FeeIC.png" => :fairy, "FeuerIC.png" => :fire, "FlugIC.png" => :flying, "GeistIC.png" => :ghost, "GesteinIC.png" => :rock, "GiftIC.png" => :poison, "KampfIC.png" => :fighting, "KäferIC.png" => :bug, "NormalIC.png" => :normal, "PflanzeIC.png" => :grass, "PsychoIC.png" => :psychic, "StahlIC.png" => :steel, "UnlichtIC.png" => :dark, "WasserIC.png" => :water}


def getTypes(string,number)
  types=string.split(" ")
  $pokemonTypesString[types[number-1]]
end

def getGeneration(number)
  generation = 1
  for i in 1..$generations.length-1 do
    if number >= $generations[i]
      generation = i+1
    end
  end
  generation
end

def getPokedata(string)
  number = 0
  germanName = 2
  englishName = 3
  frenchName = 4
  types = 8
  string.each_line{ |line| 
    line=line.split("\t")
    pokemonData = {number: line[number].strip.to_i,
                   germanName: line[germanName].strip.gsub("♀","{").gsub("♂","}").gsub("’","'"),
                   englishName: line[englishName].strip.gsub("♀","{").gsub("♂","}").gsub("’","'"),
                   frenchName: line[frenchName].strip.gsub("♀","{").gsub("♂","}").gsub("’","'"),
                   firstType: getTypes(line[types],1),
                   secondType: getTypes(line[types],2),
                   generation: getGeneration(line[number].strip.to_i),
                   megaEvolutions: [],
                   alolaForms: [],
                   legendary: false}
   

    $pokemonList.push(pokemonData)
  }
end

def getLegendaryPokemon(string)
  number = 1
  string.each_line{ |line|
    line=line.split("\t")
    pokeNumber = line[number].strip.to_i
    pokemon = $pokemonList.select{ | value | value[:number] == pokeNumber }[0]
    pokemon[:legendary] = true
  }
end

def getMegaPokemon(string)
  linesCount = 0
  lineCounter = 0
  string.each_line { linesCount += 1 }
  counter = 0;
  data = {}
  string.each_line{ |line|
    lineCounter += 1
    line=line.split("\t")
    if (line.length == 1 and line[0].include?(".png")) or lineCounter == linesCount
      if counter != 0
        pokemon = $pokemonList.select{ | value | value[:germanName] == data[:name] }[0]
        data[:number] = pokemon[:number]
        pokemon[:megaEvolutions].push(data)
        $megaList.push(data)
        data = {}
      end
      counter = 0
    elsif line[0].start_with?("Mega-")
      counter+=1
      name = line[0].strip.split(" ")
      data[:name] = name[0][5..-1].gsub("♀","{").gsub("♂","}").gsub("’","'")
      data[:firstType] = getTypes(line[1],1)
      data[:secondType] = getTypes(line[1],2)
      data[:edition] = line[8].strip.tr("αΩ","")
    elsif counter == 1
      data[:edition2] =  line[0].strip.tr("αΩ","")
      counter+=1
    end
  }
  $megaList
end

def getAlolaPokemon(string)
  counter = 0;
  currentPokemonName = "";
  data = {}
  string.each_line{ |line|
    line=line.split("\t")
    if line.length == 1 and line[0].include?(".png")
      counter = 0;
    elsif counter == 0
      currentPokemonName = line[0].strip.gsub("♀","{").gsub("♂","}").gsub("’","'")
      counter+=1
    elsif line[0].include? currentPokemonName
      data[:name] = currentPokemonName
      data[:firstType] = getTypes(line[1],1)
      data[:secondType] = getTypes(line[1],2)
      pokemon = $pokemonList.select{ | value | value[:germanName] == data[:name] }[0]
      data[:number] = pokemon[:number]
      pokemon[:alolaForms].push(data)
      $alolaList.push(data)
      data={}
    end
  }
end

def getPokemonImages(string)
  Dir.mkdir($imagesFolder)
  Dir.mkdir($imagesFolder + '/original')
  links = string.scan(/\/images\/\h+\/\h+\/Pok%C3%A9monicon_\d+.png/)
  links.each{ |link|
    imagePath = $imagesRootPath + link
    execute = 'wget --user-agent="Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36" -P "' + $imagesFolder + '/original/" "' + imagePath + '"'
    system(execute)
  }
  if links.length != $pokemonCount then puts "ERROR: there are not "+$pokemonCount.to_s+" images!" end
end

def changeImageSize
  Dir.mkdir($imagesFolder + '/sameSize')
  for pokemon in 1..$pokemonCount do
    inPath = $imagesFolder + '/original/Pokémonicon_' + pokemon.to_s.rjust(3, "0") + '.png' 
    outPath = $imagesFolder + '/sameSize/' + pokemon.to_s.rjust(3, "0") + '.png' 
    execute = 'convert "' + inPath + '" -gravity center -extent ' + $imageSize[0].to_s + 'x' + $imageSize[1].to_s + ' "' + outPath + '"'
    system(execute)
  end
end

def appendImages
  Dir.mkdir($imagesFolder + '/appended')
  for pokemon in 1..$pokemonCount do
    if (pokemon-1)% $pokemonPerImage == 0
      outPath = $imagesFolder + '/appended/' + pokemon.to_s.rjust(3, "0") + '.png' 
      execute = 'convert +append '
      $pokemonPerImage.times { |i|
        if pokemon+i > $pokemonCount
          break
        end
        execute += '"' + $imagesFolder + '/sameSize/' + (pokemon+i).to_s.rjust(3, "0") + '.png" ' 
      }
      execute += "'" + outPath + "'"
      system(execute)
    end
  end
end

def greyImages
  Dir.mkdir($imagesFolder + '/grey')
  for pokemon in 1..$pokemonCount do
    if (pokemon-1)% $pokemonPerImage == 0
      inPath = $imagesFolder + '/appended/' + pokemon.to_s.rjust(3, "0") + '.png'
      outPath = $imagesFolder + '/grey/' + pokemon.to_s.rjust(3, "0") + '.png'
      execute = 'convert ' + "'" + inPath + "'" + ' -threshold ' + $threshold.to_s + '% -background white -alpha remove ' + "'" + outPath + "'"
      system(execute)
    end
  end
end

def colorImages
  Dir.mkdir($imagesFolder + '/color')
  for pokemon in 1..$pokemonCount do
    if (pokemon-1)% $pokemonPerImage == 0
      inPath = $imagesFolder + '/appended/' + pokemon.to_s.rjust(3, "0") + '.png'
      outPath = $imagesFolder + '/color/' + pokemon.to_s.rjust(3, "0") + '~color.png'
      execute = 'convert ' + "'" + inPath + "'" + ' -dither FloydSteinberg -remap pebble_colors_64.gif -background white -alpha remove ' + "'" + outPath + "'"
      system(execute)
    end
  end
end
     
#convert in.png -dither FloydSteinberg -remap pebble_colors_64.gif png8:out.png

def resourceID
  for pokemon in 1..$pokemonCount do
    if (pokemon-1)% $pokemonPerImage == 0
      print 'RESOURCE_ID_IMAGE_POKE' + pokemon.to_s.rjust(3, "0") + ",\n"
    end
  end
end

def media
  for pokemon in 1..$pokemonCount do
    if (pokemon-1)% $pokemonPerImage == 0
      print '                {' + "\n"
      print '                    "file": "images/' + pokemon.to_s.rjust(3, "0") + '.png",' + "\n"
      print '                    "name": "IMAGE_POKE' + pokemon.to_s.rjust(3, "0") + '",' + "\n"
      print '                    "type": "bitmap",' + "\n"
      print '                    "memoryFormat": "1Bit",' + "\n"
      print '                    "spaceOptimization": "storage",' + "\n"
      print '                    "targetPlatforms": [' + "\n"
      print '                        "aplite"'+ "\n"
      print '                    ]' +"\n"
      print '                },' + "\n"
      print '                {' + "\n"
      print '                    "file": "images/' + pokemon.to_s.rjust(3, "0") + '.png",' + "\n"
      print '                    "name": "IMAGE_POKE' + pokemon.to_s.rjust(3, "0") + '",' + "\n"
      print '                    "type": "bitmap",' + "\n"
      print '                    "memoryFormat": "8Bit",' + "\n"
      print '                    "spaceOptimization": "storage",' + "\n"
      print '                    "targetPlatforms": [' + "\n"
      print '                        "basalt"'+ "\n"
      print '                    ]' +"\n"
      print '                },' + "\n"
    end
  end
end

def getImageSize
  maximum = [0,0]
  Dir.entries($imagesFolder + '/original').select {|f| !File.directory? f}.each { |image|
    path = $imagesFolder + '/original/' + image 
    dimension = IO.read(path)[0x10..0x18].unpack('NN')
    if dimension[0] > maximum[0] then maximum[0] = dimension[0] end
    if dimension[1] > maximum[1] then maximum[1] = dimension[1] end
  }
  print maximum,"\n"
end

def printPokemon(gen,lang)
  selection = $pokemonList.select{ | value | if gen == 0 then true else value[:generation] == gen end }
  string = ""
  selection.each{ |pokemon| 
    if(lang == :englishName or pokemon[lang] != pokemon[:englishName])
      string+=pokemon[lang]
    end
    string+='\0'
  }
  print('"',string.chop.chop,'"',"\n")
end

def printPokemonOrder(lang,type,legendary,megaTypes)
  list = $pokemonList.sort{ |a,b| a[lang].downcase <=> b[lang].downcase}
  string = "";
  count = 1;
  megaCount = 0;
  megaType = :firstType
  bit = 3 #Starting with 3 because array has one extra filed at the beginning
  list.each{ |pokemon| 
    selection = $pokemonList.select{ | value | value[:number] == count }[0]
    if legendary
      if selection[:legendary]
        string += "SHIFT_LEGENDARY|"
      end
    end
    if megaTypes
      typeNumberBit = 0
      alolaCount = megaCount - $megaList.length
      if megaCount < $megaList.length
        megaTypeTmp = $megaList[megaCount][megaType]
        typeNumberBit = $pokemonTypes[megaTypeTmp][bit]
        print $megaList[megaCount],' ', megaTypeTmp,' ',$pokemonTypes[megaTypeTmp],' ',typeNumberBit,"\n"
      elsif alolaCount < $alolaList.length
        alolaTypeTmp = $alolaList[alolaCount][megaType]
        typeNumberBit = $pokemonTypes[alolaTypeTmp][bit]
      end

      if typeNumberBit == 1
        string += "SHIFT_TYPE|"
      end

      if bit > 0
        bit -= 1
      else
        bit = 4
        if megaType == :firstType
          megaType = :secondType
        else
          megaType = :firstType
          megaCount += 1
        end
      end
    end

    if type
      string += "T_SHIFT_" + selection[type].to_s.upcase + "|"
    end
    string += pokemon[:number].to_s + ","
    count += 1
  }
  print('{0,',string.chop,',0}',"\n")
end

def printPokemonTypes(type)
  string = "";
  $pokemonList.each{ |pokemon|
    string += "T_" + pokemon[type].to_s.upcase + ","
  }
  print('{',string.chop,'}',"\n")
end

def printMega
  string = ""
  $megaList.each{ |megaEvolution| 
    if megaEvolution[:edition] == "R"
      string+= "MEGA_SR|"
    elsif megaEvolution[:edition] == "Y"
      string+= "MEGA_Y|"
    elsif megaEvolution[:edition] == "X" and megaEvolution[:edition2] == nil
      string+= "MEGA_X|"
    elsif megaEvolution[:edition] == "X" and megaEvolution[:edition2] == "Y"
      string+= "MEGA_ALL|"
    end
    string+= megaEvolution[:number].to_s+","
  }
  print('{',string.chop,'}',"\n")
  print "Length: ",$megaList.length,"\n"
end

def printAlola
  string = ""
  $alolaList.each{ |alolaForm| 
    string+= "ALOLA|"+ alolaForm[:number].to_s+ ","
  }
  print('{',string.chop,'}',"\n")
  print "Length: ",$alolaList.length,"\n"
end

def stringToLang(string)
  lang = :englishName
  if string == "german"
    lang = :germanName
  elsif string == "french"
    lang = :frenchName
  end
  lang
end

def stringToType(string)
  type = :firstType
  if string == "2"
    type = :secondType
  end
  type
end

def parseParameter(argv)
  if argv[0] == "help"
    puts("names <english,german,french> [1,2,3,4,5,6,7]")
    puts("order <english,german,french> [1,2] [legendary,megaTypes]")
    puts("types <1,2>")
    puts("mega")
    puts("alola")
    puts("download")
    puts("size")
    puts("changeSize")
    puts("append")
    puts("grey")
    puts("color")
    puts("ids")
    puts("media")
  elsif argv[0] == "names"
    if argv[2]
      printPokemon(argv[2].to_i,stringToLang(argv[1]))
    else
      printPokemon(0,stringToLang(argv[1]))
    end
  elsif argv[0] == "order"
    if argv.length == 4
      if argv[3] == "legendary"
        printPokemonOrder(stringToLang(argv[1]),stringToType(argv[2]),true,false)
      elsif argv[3] == "megaTypes"
        printPokemonOrder(stringToLang(argv[1]),stringToType(argv[2]),false,true)
      end
    elsif argv.length == 3
      printPokemonOrder(stringToLang(argv[1]),stringToType(argv[2]),false,false)
    elsif argv.length == 2
      printPokemonOrder(stringToLang(argv[1]),nil,false,false)
    end
  elsif argv[0] == "types"
    printPokemonTypes(stringToType(argv[1]))
  elsif argv[0] == "mega"
    printMega
  elsif argv[0] == "alola"
    printAlola
  elsif argv[0] == "download"
    getPokemonImages(IO.read($fileHtml))
  elsif argv[0] == "size"
    getImageSize
  elsif argv[0] == "changeSize"
    changeImageSize
  elsif argv[0] == "append"
    appendImages
  elsif argv[0] == "grey"
    greyImages
  elsif argv[0] == "color"
    colorImages
  elsif argv[0] == "ids"
    resourceID
  elsif argv[0] == "media"
    media
  end
end

getPokedata(IO.read($filePokemon))
getLegendaryPokemon(IO.read($fileLegendary))
getMegaPokemon(IO.read($fileMega))
getAlolaPokemon(IO.read($fileAlola))

parseParameter(ARGV)
