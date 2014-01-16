#
# Copyright (c) 2014 Sascha Steinbiss <ss23@sanger.ac.uk>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

require 'gtdlload'
require 'extended/genome_stream'

module GT
  extend DL::Importable
  gtdlload "libgenometools"
  extern "GtNodeVisitor* gt_check_boundaries_visitor_new()"

  class CheckBoundariesVisitor
    def initialize
      @genome_visitor = GT.gt_check_boundaries_visitor_new()
      @genome_visitor.free = GT::symbol("gt_node_visitor_delete", "0P")
    end

    def to_ptr
      @genome_visitor
    end
  end
end
