ARGV.each do |f|
  puts f
  src = IO.read(f)
  src = src.gsub(/\h+'\h+/){|x| puts x; x.gsub(/'/,'')}
  IO.write(f, src)
end
