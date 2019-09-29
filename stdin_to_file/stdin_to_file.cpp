#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <algorithm>

namespace {
  // Based on http://wordaligned.org/articles/cpp-streambufs
  template <typename char_type,
           typename traits = std::char_traits<char_type>
             >
             class basic_multi_teebuf:
               public std::basic_streambuf<char_type, traits>
  {
    public:
      typedef typename traits::int_type int_type;
      typedef std::basic_streambuf<char_type, traits> stream_buffer_type;
    private:
      std::vector<stream_buffer_type *> streams;
    public:
      basic_multi_teebuf(std::initializer_list<stream_buffer_type *> output_streams_buffers)
        : streams(output_streams_buffers)
      {
      }

      template <typename TIterator, typename TTransformer>
        basic_multi_teebuf(TIterator start, TIterator end, TTransformer transformer) 
        : streams(std::distance(start, end))
        {
          std::transform(start, end, std::begin(streams), transformer);
        }

    private:    
      virtual int sync()
      {
        int sync_result = 0;
        for (auto & stream_buffer_iterator : streams) {
          sync_result |= stream_buffer_iterator->pubsync();
        }

        return sync_result == 0 ? 0 : -1;
      }

      virtual int_type overflow(int_type c)
      {
        int_type const eof = traits::eof();

        if (traits::eq_int_type(c, eof))
        {
          return traits::not_eof(c);
        }
        else
        {
          char_type const ch = traits::to_char_type(c);
          bool ok = false;

          for (auto & stream_buffer_iterator : streams) {
            int_type const result = stream_buffer_iterator->sputc(ch);
            ok = traits::eq_int_type(result, eof) || ok;
          }

          return ok ? eof : c;
        }
      }
  };

  template <typename char_type,
           typename traits = std::char_traits<char_type>>
             class basic_multiteestream : public std::basic_ostream<char_type, traits>
           {
             public:
               typedef std::basic_ostream<char_type, traits> stream_type;
               typedef std::basic_streambuf<char_type, traits> stream_buffer_type;
               template <typename... TStream>
                 basic_multiteestream(TStream &...ostreams)
                 : std::basic_ostream<char_type, traits>(&tbuf)
                   , tbuf({(ostreams.rdbuf())...})
                   {}

               template <typename TIterator>
                 basic_multiteestream(TIterator start, TIterator end)
                 : std::basic_ostream<char_type, traits>(&tbuf)
                   , tbuf(start, end,
                       [](typename std::iterator_traits<TIterator>::value_type &stream_iterator) { return stream_iterator.rdbuf(); }
                       )
                 {}
             private:
               basic_multi_teebuf<char_type, traits> tbuf;
           };

  typedef basic_multiteestream<char> multiteestream;
}

int main(int argc, char **argv) {
  std::vector<std::string> filenames;

  for (int arg_iterator = 1; arg_iterator < argc; ++arg_iterator) {
    std::string const arg(argv[arg_iterator]);

    if (arg == "-h" || arg == "--help" || arg == "/?") {
      std::cout << "stdin_to_file [-o] OUTPUT_FILE [[-o ] OUTPUT_FILE2...]\n  Redirect stdin to files\n"
        "Parameters:\n"
        "-h|--help|/? Show this help.\n"
        "[-o] OUTPUT_FILE File to write output to. Can be specified multiple times\n";
    } else if (arg == "-o") {
      if (arg_iterator == argc - 1) {
        std::cerr << "Expected output file argument at position " << argc << std::endl;
        return 1;
      }
      ++arg_iterator;
      filenames.push_back(argv[arg_iterator]);
    } else {
      filenames.push_back(arg);
    }
  }

  std::vector<std::ofstream> files(filenames.size());
  std::transform(std::begin(filenames), std::end(filenames), std::begin(files), [](auto name) { return std::ofstream(name, std::ios_base::binary); });

  multiteestream tee_output(std::begin(files), std::end(files));
  std::ostream &output = tee_output;
  output << std::cin.rdbuf();

  return 0;
}



