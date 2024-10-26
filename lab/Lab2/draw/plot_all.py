import multiprocessing
import subprocess
import os
import sys

def run_plot_script(file_path, directory):
    # Determine the directory of the current script
    if directory == "text":
        plot_script_path = "plot.py"
    else:
        run_path = directory.split("/text")[0]
        plot_script_path = os.path.join(run_path, 'plot.py')
    
    # Run plot.py with the given file path
    subprocess.run(['python3', plot_script_path, file_path])

def get_all_files_in_directory(directory):
    return [os.path.join(directory, f) for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]

if __name__ == "__main__":
    if len(sys.argv) >= 2:
        directory = sys.argv[1]
    else:
        directory = "text"
    print(directory)
    
    # Get all files in the specified directory
    file_paths = get_all_files_in_directory(directory)
    
    # Prepare arguments for starmap
    args = [(file_path, directory) for file_path in file_paths]
    print(directory)
    
     # Determine the number of available cores for optimal parallelism
    num_cores = min(multiprocessing.cpu_count(), len(file_paths))
    
    # Use multiprocessing to run plot.py in parallel
    with multiprocessing.Pool(num_cores) as pool:
        pool.starmap(run_plot_script, args)