/* llama.i */
/*
    This file is part of guile_llama_cpp.

    guile_llama_cpp is free software: you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    guile_llama_cpp is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with guile_llama_cpp. If not, see <https://www.gnu.org/licenses/>.

*/

%module "guile-llama-cpp"
%{
#include "llama.h"
#include <vector>
void llama_batch_add(
                 struct llama_batch & batch,
                        llama_token   id,
                          llama_pos   pos,
    const std::vector<llama_seq_id> & seq_ids,
                               bool   logits) {
    batch.token   [batch.n_tokens] = id;
    batch.pos     [batch.n_tokens] = pos;
    batch.n_seq_id[batch.n_tokens] = seq_ids.size();
    for (size_t i = 0; i < seq_ids.size(); ++i) {
        batch.seq_id[batch.n_tokens][i] = seq_ids[i];
    }
    batch.logits  [batch.n_tokens] = logits;

    batch.n_tokens++;
}

llama_token_data * llama_token_data_vector_data(std::vector<llama_token_data> &array)  {
    return array.data();
}
%}

%include "guile_typemap.i"

%typemap(in) (char *buf, int32_t length) {
    $1 = (char*) SCM_BYTEVECTOR_CONTENTS($input);
    $2 = SCM_BYTEVECTOR_LENGTH($input);
}

%typemap(in) (const char * text, size_t text_len) {
  size_t temp;
  $1 = SWIG_Guile_scm2newstr($input,&temp);
  $2 = temp + 1; // for some reason the temp is set to strlen()input) -1, see ./Lib/guile/guile_scm_run.swg
}
%apply (const char * text, size_t text_len) { (const char * text, int32_t text_len) };

%array_class(llama_token, llama_token_array);  // Make a typemap for arrays of llama_token
%array_class(llama_token_data, llama_token_data_carray);  // Make a typemap for C style arrays of llama_token
 // note use "carray" instead of "array" to avoid name clash with existing struct llama_token_data_array

%include "llama_c_header.h"

namespace std {
    %template(llama_seq_id_vector) vector<llama_seq_id>;
    %template(llama_token_data_vector) vector<llama_token_data>;
}
extern void llama_batch_add(
                 struct llama_batch & batch,
                        llama_token   id,
                          llama_pos   pos,
    const std::vector<llama_seq_id> & seq_ids,
                               bool   logits);
extern llama_token_data * llama_token_data_vector_data(std::vector<llama_token_data> &array);
