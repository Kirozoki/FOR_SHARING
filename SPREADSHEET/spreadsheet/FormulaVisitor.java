// Generated from Formula.g4 by ANTLR 4.13.2
import org.antlr.v4.runtime.tree.ParseTreeVisitor;

/**
 * This interface defines a complete generic visitor for a parse tree produced
 * by {@link FormulaParser}.
 *
 * @param <T> The return type of the visit operation. Use {@link Void} for
 * operations with no return type.
 */
public interface FormulaVisitor<T> extends ParseTreeVisitor<T> {
	/**
	 * Visit a parse tree produced by {@link FormulaParser#main}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitMain(FormulaParser.MainContext ctx);
	/**
	 * Visit a parse tree produced by the {@code UnaryOp}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitUnaryOp(FormulaParser.UnaryOpContext ctx);
	/**
	 * Visit a parse tree produced by the {@code Parens}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitParens(FormulaParser.ParensContext ctx);
	/**
	 * Visit a parse tree produced by the {@code Literal}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitLiteral(FormulaParser.LiteralContext ctx);
	/**
	 * Visit a parse tree produced by the {@code Cell}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitCell(FormulaParser.CellContext ctx);
	/**
	 * Visit a parse tree produced by the {@code BinaryOp}
	 * labeled alternative in {@link FormulaParser#expr}.
	 * @param ctx the parse tree
	 * @return the visitor result
	 */
	T visitBinaryOp(FormulaParser.BinaryOpContext ctx);
}