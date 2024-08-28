#!/usr/bin/bash                                             

#SBATCH --ntasks=1                    # Run on a single CPU
#SBATCH --cpus-per-task=2             # Run on a single CPU
#SBATCH --mem=1gb                     # Job memory request
#SBATCH --time=2-00:00:00             # Time limit 1 day
#SBATCH --output=LOGDIR_NAME/LOGOUT   # Standard output
#SBATCH --error=ERRDIR_NAME/ERROUT    # Standard error
#SBATCH --partition=n1-long          # which queue

start=$(date +%s)

PROG MACRO OUTPUTFILE SEEDNUMBER

end=$(date +%s)
elapsed=$(( end - start ))

echo "Total time taken: $elapsed seconds"

