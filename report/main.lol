\contentsline {lstlisting}{\numberline {1}DFSA Class Declaration (lexer/DFSA.hpp)}{1}{lstlisting.0.1}%
\contentsline {lstlisting}{\numberline {2}Code specification of the comments in the \texttt {LexerDirector} (lexer/LexerDirector.cpp)}{7}{lstlisting.0.2}%
\contentsline {lstlisting}{\numberline {3}Registration of the hexadecimal category checker (lexer/LexerDirector.cpp)}{7}{lstlisting.0.3}%
\contentsline {lstlisting}{\numberline {4}Constructions of the Lexer (lexer/LexerBuilder.cpp)}{8}{lstlisting.0.4}%
\contentsline {lstlisting}{\numberline {5}The \texttt {updateLocationState()} lexer method (lexer/Lexer.cpp)}{8}{lstlisting.0.5}%
\contentsline {lstlisting}{\numberline {6}Error handling mechanism in the \texttt {nextToken()} lexer method (lexer/Lexer.cpp)}{9}{lstlisting.0.6}%
\contentsline {lstlisting}{\numberline {7}The Runner constructor passes \texttt {mLexer} into the Parser constructor (runner/Runner.cpp)}{10}{lstlisting.0.7}%
\contentsline {lstlisting}{\numberline {8}Mechanism for internally storing types within the compiler (parl/Core.hpp)}{14}{lstlisting.0.8}%
\contentsline {lstlisting}{\numberline {9}The Primitive structure (parl/Core.hpp)}{15}{lstlisting.0.9}%
\contentsline {lstlisting}{\numberline {10}An size unbounded type in {C\nolinebreak \raisebox {.25ex}{\scriptsize \bfseries {++}}}{} (parl/Core.hpp)}{15}{lstlisting.0.10}%
\contentsline {lstlisting}{\numberline {11}The \texttt {FunctionCall} AST node class (parl/AST.hpp)}{17}{lstlisting.0.11}%
\contentsline {lstlisting}{\numberline {12}The \texttt {Binary} AST node class (parl/AST.hpp)}{17}{lstlisting.0.12}%
\contentsline {lstlisting}{\numberline {13}The \texttt {Unary} AST node class (parl/AST.hpp)}{17}{lstlisting.0.13}%
\contentsline {lstlisting}{\numberline {14}The \texttt {moveWindow()} and \texttt {nextToken()} Parser methods (parser/Parser.cpp)}{19}{lstlisting.0.14}%
\contentsline {lstlisting}{\numberline {15}The \texttt {consume()} Parser methods (parser/Parser.hpp)}{20}{lstlisting.0.15}%
\contentsline {lstlisting}{\numberline {16}The Usage of the \texttt {consume()} method in the \texttt {formalParam()} generator method (parser/Parser.cpp)}{20}{lstlisting.0.16}%
\contentsline {lstlisting}{\numberline {17}The \texttt {peek()} Parser method (parser/Parser.cpp)}{21}{lstlisting.0.17}%
\contentsline {lstlisting}{\numberline {18}The abort functionality present in the codebase (parl/Core.hpp)}{21}{lstlisting.0.18}%
\contentsline {lstlisting}{\numberline {19}The \texttt {ParseSync} exception and the \texttt {error()} method which kickstarts the synchronization process (parser/Parser.hpp)}{22}{lstlisting.0.19}%
\contentsline {lstlisting}{\numberline {20}The \texttt {synchronize()} method in the Parser class (parser/Parser.cpp)}{23}{lstlisting.0.20}%
\contentsline {lstlisting}{\numberline {21}The \texttt {ifStmt()} node generator method in the Parser class (parser/Parser.cpp)}{25}{lstlisting.0.21}%
\contentsline {lstlisting}{\numberline {22}The main body of methods in the Parser class (parser/Parser.hpp)}{26}{lstlisting.0.22}%
\contentsline {lstlisting}{\numberline {23}The parse segment of the \texttt {run()} method in the Runner class (runner/Runner.cpp)}{27}{lstlisting.0.23}%
\contentsline {lstlisting}{\numberline {24}The \texttt {debugParsing()} method in the Runner class (runner/Runner.cpp)}{29}{lstlisting.0.24}%
\contentsline {lstlisting}{\numberline {25}A segment of the pure virtual \texttt {Visitor} class (parl/Visitor.hpp)}{30}{lstlisting.0.25}%
\contentsline {lstlisting}{\numberline {26}The \texttt {visit(core::PadRead*)} method in the \texttt {PrinterVisitor} (parser/PrinterVisitor.cpp)}{31}{lstlisting.0.26}%
\contentsline {lstlisting}{\numberline {27}The \texttt {Environment} class with \texttt {mChildren} highlighted (backend/Environment.hpp)}{35}{lstlisting.0.27}%
\contentsline {lstlisting}{\numberline {28}The \texttt {pushEnv()} method in the \texttt {EnvStack} class (analysis/EnvStack.cpp)}{37}{lstlisting.0.28}%
\contentsline {lstlisting}{\numberline {29}The \texttt {popEnv()} method in the \texttt {EnvStack} class (analysis/EnvStack.cpp)}{37}{lstlisting.0.29}%
\contentsline {lstlisting}{\numberline {30}The \texttt {pushEnv()} method in the \texttt {RefStack} class (ir\_gen/RefStack.cpp)}{37}{lstlisting.0.30}%
\contentsline {lstlisting}{\numberline {31}The \texttt {popEnv()} method in the \texttt {RefStack} class (ir\_gen/RefStack.cpp)}{38}{lstlisting.0.31}%
\contentsline {lstlisting}{\numberline {32}Definitions of \texttt {VariableSymbol} and \texttt {FunctionSymbol} (backend/Symbol.hpp)}{40}{lstlisting.0.32}%
\contentsline {lstlisting}{\numberline {33}The \texttt {visit(FormalParam *)} method in the \texttt {AnalysisVisitor} class (analysis/AnalysisVisitor.cpp)}{41}{lstlisting.0.33}%
\contentsline {lstlisting}{\numberline {34}The \texttt {visit(Variable *)} method in the \texttt {AnalysisVisitor} class (analysis/AnalysisVisitor.cpp)}{43}{lstlisting.0.34}%
\contentsline {lstlisting}{\numberline {35}A part of the \texttt {visit(Binary *)} method in the \texttt {AnalysisVisitor} class (analysis/AnalysisVisitor.cpp)}{43}{lstlisting.0.35}%
\contentsline {lstlisting}{\numberline {36}Another part of the \texttt {visit(Binary *)} method in the \texttt {AnalysisVisitor} class (analysis/AnalysisVisitor.cpp)}{44}{lstlisting.0.36}%
\contentsline {lstlisting}{\numberline {37}Checking whether return statement is inside a function declaration in the \texttt {visit(ReturnStmt *)} method in the \texttt {AnalysisVisitor} class (analysis/AnalysisVisitor.cpp)}{45}{lstlisting.0.37}%
\contentsline {lstlisting}{\numberline {38}Checking whether the return expression has the same type as the function return type in the \texttt {visit(ReturnStmt *)} method in the \texttt {AnalysisVisitor} class (analysis/AnalysisVisitor.cpp)}{45}{lstlisting.0.38}%
\contentsline {lstlisting}{\numberline {39}The \texttt {visit(IfStmt *)} method in the \texttt {ReturnVisitor} class (analysis/ReturnVisitor.cpp)}{47}{lstlisting.0.39}%
\contentsline {lstlisting}{\numberline {40}The \texttt {visit(Block *)} method in the \texttt {ReturnVisitor} class (analysis/ReturnVisitor.cpp)}{47}{lstlisting.0.40}%
\contentsline {lstlisting}{\numberline {41}Getting the global environment from the \texttt {AnalysisVisitor} and passing it on for reordering and code generation (runner/Runner.cpp)}{50}{lstlisting.0.41}%
\contentsline {lstlisting}{\numberline {42}The \texttt {visit(FunctionCall *)} method in the \texttt {TypeVisitor} class (ir\_gen/TypeVisitor.cpp)}{51}{lstlisting.0.42}%
