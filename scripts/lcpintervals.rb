#!/usr/bin/env ruby

def assert
  raise "Assertion failed !" unless yield
end

Lcpinterval = Struct.new("Lcpinterval",:lcp, :lb, :rb, :brchildlist, :hasedge)

class Lcpstream
  def initialize(filename)
    @lcpfile = File.new(filename + ".lcp","r")
    @lcpfile.read(1)
    @llvfile = File.new(filename + ".llv","r")
  end
  def nextlcp()
    @lcpfile.each_byte do |cc|
      if cc == 255
        contents = @llvfile.read(4)
        contents = @llvfile.read(4)
        yield contents.unpack("L")[0]
      else
        yield cc
      end
    end
  end
end

def processlcpinterval(itv)
  puts "N #{itv.lcp} #{itv.lb} #{itv.rb}"
end

def enumlcpintervals(filename)
  stack = Array.new()
  stack.push(Lcpinterval.new(0,0,nil,[],false))
  idx = 1
  Lcpstream.new(filename).nextlcp() do |lcpvalue|
    lb = idx - 1
    while lcpvalue < stack.last.lcp
      lastinterval = stack.pop
      lastinterval.rb = idx-1
      processlcpinterval(lastinterval)
      lb = lastinterval.lb
    end
    if lcpvalue > stack.last.lcp
      stack.push(Lcpinterval.new(lcpvalue,lb,nil,[],false))
    end
    idx += 1
  end
  lastinterval = stack.pop
  lastinterval.rb = idx-1
  processlcpinterval(lastinterval)
end

def processbranchingedge(fromitv,toitv)
  puts "B #{fromitv.lcp} #{fromitv.lb} #{toitv.lcp} #{toitv.lb}"
end

def processleaves(nextleaf,fatherlcp,fatherlb,startpos,endpos)
  startpos.upto(endpos) do |idx|
    puts "L #{fatherlcp} #{fatherlb} #{idx}"
  end
  return [endpos+1,nextleaf].max
end

def showbool(b)
  if b
    return "1"
  else
    return "0"
  end
end

def processleavesfromqueue(hasedge,fatherlcp,fatherlb,queue,endpos)
  out = false
  firstedge = true 
  if hasedge 
    firstedge = false 
  end
  queue.enumleaves(endpos) do |leaf|
    puts "L #{showbool(firstedge)} #{fatherlcp} #{fatherlb} #{leaf}"
    firstedge = false
    out = true
  end
  return out
end

def add_to_top_brchildlist(stack,itv)
  topelem = stack.pop
  topelem.brchildlist.push(itv)
  topelem.hasedge = true
  stack.push(topelem)
end

def addleaftail(nextleaf,itv)
  return processleaves(
               nextleaf,
               itv.lcp,
               itv.lb,
               if itv.brchildlist.empty? then itv.lb
                                         else itv.brchildlist.last.rb+1 end,
               itv.rb)
end

def enumlcpintervaltree(filename)
  stack = Array.new()
  lastinterval = nil
  stack.push(Lcpinterval.new(0,0,nil,[],false))
  nextleaf = 0
  idx = 1
  Lcpstream.new(filename).nextlcp() do |lcpvalue|
    lb = idx - 1
    while lcpvalue < stack.last.lcp
      lastinterval = stack.pop
      lastinterval.rb = idx - 1
      nextleaf = addleaftail(nextleaf,lastinterval)
      lb = lastinterval.lb
      if lcpvalue <= stack.last.lcp
        add_to_top_brchildlist(stack,lastinterval)
        processbranchingedge(stack.last,lastinterval)
        lastinterval = nil
      end
    end
    if lcpvalue > stack.last.lcp
      if lastinterval.nil?
        nextleaf = processleaves(nextleaf,stack.last.lcp,stack.last.lb,
                              nextleaf,lb-1)
        stack.push(Lcpinterval.new(lcpvalue,lb,nil,[],false))
      else
        stack.push(Lcpinterval.new(lcpvalue,lb,nil,[lastinterval],false))
        processbranchingedge(stack.last,lastinterval)
        lastinterval = nil
      end
    end
    idx += 1
  end
  lastinterval = stack.pop
  lastinterval.rb = idx - 1
  nextleaf = addleaftail(nextleaf,lastinterval)
end

class Queuearray
  def initialize()
    @space = Array.new()
    @dist1 = @dist2 = 0
  end
  def delete()
    puts "# dist: 1=#{@dist1},2=#{@dist2}"
  end
  def add(elem)
    assert {@space.length < 2}
    @space.push(elem)
    if @space.length == 1
      @dist1 += 1
    else
      @dist2 += 1
    end
  end
  def enumleaves(endpos)
    while not @space.empty?
      idx = @space.shift
      if idx <= endpos
        yield idx
      else
        @space.unshift(idx)
        break
      end
    end
  end
end

class Queuepair
  def initialize()
    @rightelem = nil
    @leftelem = nil
  end
  def delete()
    return
  end
  def add(elem)
    assert {@leftelem.nil?}
    @leftelem = @rightelem
    @rightelem = elem
  end
  def enumleaves(endpos)
    if not @leftelem.nil?
      if @leftelem <= endpos
        yield @leftelem
        @leftelem = nil
      else
        return
      end
    end
    if not @rightelem.nil?
      if @rightelem <= endpos
        yield @rightelem
        @rightelem = nil
      else
        return
      end
    end
  end
end

def enumlcpintervaltreewithqueue(filename)
  stack = Array.new()
  queue = Queuearray.new()
  lastinterval = nil
  stack.push(Lcpinterval.new(0,0,nil,[],false))
  idx=1
  Lcpstream.new(filename).nextlcp() do |lcpvalue|
    lb = idx - 1
    queue.add(idx - 1)
    while lcpvalue < stack.last.lcp
      lastinterval = stack.pop
      lastinterval.rb = idx - 1
      out = processleavesfromqueue(lastinterval.hasedge,
                                   lastinterval.lcp,lastinterval.lb,queue,idx-1)
      lastinterval.hasedge = true if out
      lb = lastinterval.lb
      if lcpvalue <= stack.last.lcp
        add_to_top_brchildlist(stack,lastinterval)
        processbranchingedge(stack.last,lastinterval)
        lastinterval = nil
      end
    end
    if lcpvalue > stack.last.lcp
      if lastinterval.nil?
        out = processleavesfromqueue(stack.last.hasedge,
                                     stack.last.lcp,stack.last.lb,queue,lb-1)
        stack.last.hasedge = true if out
        stack.push(Lcpinterval.new(lcpvalue,lb,nil,[],false))
      else
        stack.push(Lcpinterval.new(lcpvalue,lb,nil,[lastinterval],true))
        processbranchingedge(stack.last,lastinterval)
        lastinterval = nil
      end
    else
      out = processleavesfromqueue(stack.last.hasedge,
                                   stack.last.lcp,stack.last.lb,queue,idx-1)
      stack.last.hasedge = true if out
    end
    idx += 1
  end
  lastinterval = stack.pop
  lastinterval.rb = idx - 1
  queue.add(idx-1)
  out = processleavesfromqueue(lastinterval.hasedge,
                               lastinterval.lcp,lastinterval.lb,queue,idx-1)
  lastinterval.hasedge = true if out
  queue.delete()
end

if ARGV.length != 2
  STDERR.puts "Usage: #{$0} (itv|tree|debugtree) <indexname>"
  exit 1
end

if ARGV[0] == 'itv'
  enumlcpintervals(ARGV[1])
elsif ARGV[0] == 'tree'
  enumlcpintervaltree(ARGV[1])
elsif ARGV[0] == 'debugtree'
  enumlcpintervaltreewithqueue(ARGV[1])
end
