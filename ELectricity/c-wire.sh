#!/bin/bash

# Utilisation : ./c-wire.sh <chemin_fichier_donnees> <type_station> <type_consommateur> [id_centrale] [-h]

# Fonction pour afficher l'aide
show_help() {
    echo "Usage: $0 <data_file_path> <station_type> <consumer_type> [plant_id] [-h]"
    echo "  <data_file_path>: Path to the input CSV file."
    echo "  <station_type>: hvb | hva | lv"
    echo "  <consumer_type>: comp | indiv | all"
    echo "  [plant_id]: (Optional) Plant ID for filtering."
    echo "  -h: Display help."
}

# Gérer l'option d'aide
if [[ "$*" == *"-h"* ]]; then
    show_help
    exit 0
fi

# Valider les arguments d'entrée
if [[ $# -lt 3 ]]; then
    echo "Error: Insufficient arguments provided."
    show_help
    exit 1
fi

DATA_FILE=$1
STATION_TYPE=$2
CONSUMER_TYPE=$3
PLANT_ID=$4

# Vérifier les contraintes de type de station et de consommateur
if [[ "$STATION_TYPE" == "hvb" && "$CONSUMER_TYPE" != "comp" ]] || [[ "$STATION_TYPE" == "hva" && "$CONSUMER_TYPE" != "comp" ]]; then
    echo "Error: Invalid station and consumer type combination."
    exit 1
fi

# S'assurer que le fichier d'entrée existe
if [[ ! -f "$DATA_FILE" ]]; then
    echo "Error: Data file not found at $DATA_FILE."
    exit 1
fi

# Préparer les répertoires
mkdir -p tmp graphs tests

# Nettoyer le répertoire tmp
rm -f tmp/*

# Compiler le programme C si nécessaire
cd codeC || exit
if [[ ! -f cwire ]]; then
    mingw32-make || { echo "Error: Failed to compile C program."; exit 1; }
fi
cd ..

FILTERED_FILE="tmp/filtered_data.csv"
awk -F',' -v stype="$STATION_TYPE" -v ctype="$CONSUMER_TYPE" -v pid="$PLANT_ID" '
BEGIN { OFS="," }
NR > 1 {  # Skip the header row
    valid_station = 0;
    if (stype == "hvb" && $2 != "-") valid_station = 1;
    else if (stype == "hva" && $3 != "-") valid_station = 1;
    else if (stype == "lv" && $4 != "-") valid_station = 1;

    valid_consumer = 0;
    if (ctype == "comp" && $5 != "-") valid_consumer = 1;
    else if (ctype == "indiv" && $6 != "-") valid_consumer = 1;
    else if (ctype == "all") valid_consumer = 1;

    valid_plant = (pid == "" || $1 == pid);

    if (valid_station && valid_consumer && valid_plant) {
        print $1, $2, $3, $4, $5, $6, $7, $8;
    }
}' "$DATA_FILE" > "$FILTERED_FILE"

# Run C program
RESULT_FILE="tests/${STATION_TYPE}_${CONSUMER_TYPE}_${PLANT_ID}.csv"
./codeC/cwire "$FILTERED_FILE" "$RESULT_FILE"

# Generate graphs for LV station type with "all" consumer
if [[ "$STATION_TYPE" == "lv" && "$CONSUMER_TYPE" == "all" ]]; then
    TOP_FILE="tests/lv_all_minmax.csv"
    ./codeC/cwire --minmax "$FILTERED_FILE" "$TOP_FILE"

    gnuplot -e "
        set terminal png size 1024,768;
        set output 'graphs/lv_all_minmax.png';
        set title 'Top 10 Most and Least Loaded LV Stations';
        set style data histograms;
        set style histogram cluster gap 1;
        set style fill solid;
        set ylabel 'Load (kWh)';
        set xtics rotate by -45;
        plot '$TOP_FILE' using 2:xtic(1) title 'Capacity', '' using 3 title 'Load';
    "
fi

echo "Processing complete. Results saved in the 'tests/' folder."
