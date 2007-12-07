#
# Copyright (c) 2007 Gordon Gremme <gremme@zbh.uni-hamburg.de>
# Copyright (c) 2007 Center for Bioinformatics, University of Hamburg
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

require 'dl/import'
require 'libgtcore/array'

module GT
  extend DL::Importable
  dlload "libgtview.so"
  extern "FeatureIndex* feature_index_new()"
  extern "void feature_index_delete(FeatureIndex*)"
  extern "Array* feature_index_get_features_for_seqid(FeatureIndex*, const " +
                                                     "char*)"

  class FeatureIndex
    attr_reader :feature_index
    def initialize
      @feature_index = GT.feature_index_new()
    end

    def get_features_for_seqid(seqid)
      rval = GT.feature_index_get_features_for_seqid(self.feature_index, seqid)
      if rval then
        a = GT::Array.new(rval)
        result = []
        1.upto(a.size) do |i|
          result.push(GT::GenomeNode.new(a.get(i-1)))
        end
        result
      else
        nil
      end
    end
  end
end
