#!/usr/bin/perl

use strict;
use warnings;
use POSIX qw(strftime);

my $log_dir = "logs_top";
my $err_dir = "errors_top";
my $macro_file = "gun_top.mac";
my $beam_number = 10000;
my $job_number = 15;


for my $i (1..$job_number) {

    my $current_time = time();
    my $shortened_time = $current_time % 100000;
    my $seed_number = "${shortened_time}${i}";
    my $output_file = "data_top/output_${i}.root";

    my $photon_path = "/opt/geant4-v10.7.4/hawl2/top_photon.txt";
        
    my $macro_path = "/opt/geant4-v10.7.4/hawl2/$macro_file";
    open my $fh, '<', $macro_path or die "Cannot open $macro_path: $!";
    my @lines = <$fh>;
    close $fh;
    
    for my $line (@lines) {
	$line =~ s{/run/beamOn \d+ top_photon\.txt}{/run/beamOn $beam_number top_photon.txt};
    }
    
    open my $fh_out, '>', $macro_path or die "Cannot write to $macro_path: $!";
    print $fh_out @lines;
    close $fh_out;
    
    my $temp_sbatch_file = "top_${i}.sh";
    open my $in, '<', 'global_sbatch.sh' or die "Cannot open global_sbatch.sh: $!";
    open my $out, '>', $temp_sbatch_file or die "Cannot open $temp_sbatch_file: $!";
    
    while (my $line = <$in>) {
	$line =~ s/LOGDIR_NAME/$log_dir/g;
	$line =~ s/LOGOUT/${i}/g;
	$line =~ s/ERRDIR_NAME/$err_dir/g;
	$line =~ s/ERROUT/${i}/g;
	$line =~ s/MACRO/\/opt\/geant4-v10.7.4\/hawl2\/$macro_file/g;
	$line =~ s/OUTPUTFILE/$output_file/g;
	$line =~ s/SEEDNUMBER/$seed_number/g;
	$line =~ s/PROG/\/opt\/geant4-v10.7.4\/hawl2\/build\/Hawl/g;
	print $out $line;
    }
    
    close $in;
    close $out;
    
    system("sbatch $temp_sbatch_file");
    unlink $temp_sbatch_file;
    
    select(undef, undef, undef, 0.25); 
}
