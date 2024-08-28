#!/usr/bin/perl

use strict;
use warnings;
use POSIX qw(strftime);

my $log_dir = "logs";
my $err_dir = "errors_cyl";
my $macro_file = "gun.mac";
my $beam_number = 10;
my $job_number = 3;


my @y_positions = (2.5, 12.5, 22.5);

my @x_positions = map { 2.5 + 5 * $_ } 0..9;

for my $i (1..$job_number) {

    my $x_index = ($i - 1) % @x_positions;
    my $y_index = int(($i - 1) / @x_positions) % @y_positions;

    my $x_position = $x_positions[$x_index];
    my $y_position = $y_positions[$y_index];

    my $unique_id = "${x_position}_${y_position}";
    my $current_time = time();
    my $shortened_time = $current_time % 100000;
    my $seed_number = "${shortened_time}${i}";
    my $output_file = "data_cyl/output_${unique_id}.root";
    
    my $cylinder_photon_path = "/opt/geant4-v10.7.4/hawl2/cylinder_photon.txt";
    open my $fh_photon, '<', $cylinder_photon_path or die "Cannot open $cylinder_photon_path: $!";
    my @photon_lines = <$fh_photon>;
    close $fh_photon;

    for my $line (@photon_lines) {
        if ($line =~ /\/gps\/pos\/centre \S+ \S+ \S+ cm/) {
            $line = "/gps/pos/centre $x_position $y_position 0. cm\n"; # Update position
        }
    }

    open my $fh_photon_out, '>', $cylinder_photon_path or die "Cannot write to $cylinder_photon_path: $!";
    print $fh_photon_out @photon_lines;
    close $fh_photon_out;

    my $macro_path = "/opt/geant4-v10.7.4/hawl2/build/$macro_file";
    open my $fh, '<', $macro_path or die "Cannot open $macro_path: $!";
    my @lines = <$fh>;
    close $fh;
    
    for my $line (@lines) {
	$line =~ s{/run/beamOn \d+ cylinder_photon\.txt}{/run/beamOn $beam_number cylinder_photon.txt};
    }
    
    open my $fh_out, '>', $macro_path or die "Cannot write to $macro_path: $!";
    print $fh_out @lines;
    close $fh_out;
    
    my $temp_sbatch_file = "cyl_${unique_id}.sh";
    open my $in, '<', 'global_sbatch.sh' or die "Cannot open global_sbatch.sh: $!";
    open my $out, '>', $temp_sbatch_file or die "Cannot open $temp_sbatch_file: $!";
    
    while (my $line = <$in>) {
	$line =~ s/LOGDIR_NAME/$log_dir/g;
	$line =~ s/LOGOUT/$unique_id/g;
	$line =~ s/ERRDIR_NAME/$err_dir/g;
	$line =~ s/ERROUT/$unique_id/g;
	$line =~ s/MACRO/\/opt\/geant4-v10.7.4\/hawl2\/build\/$macro_file/g;
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
