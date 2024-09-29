# plot all the txt files in /layout 
# and save the plots in /drawing

# create the directory if it doesn't exist
mkdir -p drawing

# plot all the txt files in /layout
for file in layout/*.txt
do
    # get the filename without the extension
    filename=$(basename -- "$file")
    filename="${filename%.*}"
    # plot the file and save it in /drawing
    python draw_block_layout.py $file drawing/$filename.png
done