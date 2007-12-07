Name "genome_stream bindings (output stream)"
Keywords "gt_ruby"
Test do
  run_ruby "#{$testdata}/gtruby/gff3.rb #{$testdata}/gff3_file_1_short.txt"
  run "env LC_ALL=C sort #{$last_stdout}"
  run "diff #{$last_stdout} #{$testdata}gff3_file_1_short_sorted.txt"
end

Name "genome_visitor bindings (output stream)"
Keywords "gt_ruby"
Test do
  run_ruby "#{$testdata}/gtruby/genome_visitor.rb #{$testdata}/gff3_file_1_short.txt"
  run "env LC_ALL=C sort #{$last_stdout}"
  run "diff #{$last_stdout} #{$testdata}gff3_file_1_short_sorted.txt"
end

if $arguments["libgtview"] then
  Name "feature_index and feature_stream bindings"
  Keywords "gt_ruby"
  Test do
    run_ruby "#{$testdata}/gtruby/feature_stuff.rb #{$testdata}"
    run "env LC_ALL=C sort #{$last_stdout}"
    run "grep -v '^##sequence-region' #{$testdata}gff3_file_1_short_sorted.txt | diff #{$last_stdout} -"
  end
end
