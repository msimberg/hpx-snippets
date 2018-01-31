using Colors
using DataFrames
using Gadfly

function collect_data(x_in, x_out, rest)
    df_combined = DataFrame()

    df_combined[x_out] = readtable(rest[1, 1])[x_in]

    for idx in 1:size(rest)[1]
        df_combined[rest[idx, 3]] = readtable(rest[idx, 1])[rest[idx, 2]]
    end

    return df_combined
end

function plot_data(df)
    return plot(
        df,
        x = names(df)[1],
        y = Col.value(names(df)[2:end]...),
        color = Col.index(names(df)[2:end]...),
        Geom.line,
        Scale.y_log10)
end
