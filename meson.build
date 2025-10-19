project('my-cpp-project', 'cpp',
    version: '1.0.0',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3',
        'werror=true', # Treat all warnings as errors
        'optimization=s' # Optimize for size
    ]
)

cpp_args = [
    # Recommended warnings for g++/clang
    '-Wpedantic',
    '-Wformat=2',
    '-Wundef',
    '-Wcast-align',
    '-Wwrite-strings',
    '-Wold-style-cast',
    '-Wnull-dereference',
    '-Wctor-dtor-privacy',
    '-Wno-missing-field-initializers',
]

add_global_arguments(cpp_args, language : 'cpp')

# Include directory untuk header files di folder resc
inc = include_directories('resc')

app_sources = [
    'main.cpp',
    'resc/bank_customer.cpp',
    'resc/buyer.cpp',
    'resc/transaction.cpp',
    'resc/datetime.cpp',
    'resc/persistence.cpp',
]

executable('my_app',
    app_sources,
    include_directories: inc,
    install: true
)